Rain = {}
Rain.__index = Rain
function Rain:Create(params)
	params = params or {} -- If no params are sent in, use the default values
	local this = 
	{
		m_InitialPosition = params.position or vec2(0, 0),
		m_Velocity = params.velocity or vec2(200, 200),
		m_MinVelocity = params.min_velocity or 100,
		m_MaxVelocity = params.max_velocity or 400,
		m_LifeTime = params.life_time or 1000, -- In mS 
		m_sTexture = params.texture or "rain",
		m_Width = params.width or 32,
		m_Height = params.height or 16,
		m_Alpha = params.alpha or 135, -- semi-transparent
		m_Scale = params.scale or 1,
		m_NumFrames = params.num_frames or 5, 
		m_MinLifeTime = params.min_life_time or 0,
		m_MaxLifeTime = params.max_life_time or 1500,
		m_LifeTimer = Timer()
	}

	this.m_bStartFinish = false

	-- Create the entity
	this.m_Entity = Entity("", "rain")
	this.m_Entity:add_component(Transform(this.m_InitialPosition, vec2(this.m_Scale, this.m_Scale), 0))
	local sprite = this.m_Entity:add_component(Sprite(this.m_sTexture, this.m_Width, this.m_Height, 0, 0, 6))
	sprite.color = Color(255, 255, 255, this.m_Alpha)
	sprite:generate_uvs() 

	local animation = this.m_Entity:add_component(Animation(0, 10, 0, false, false))
	this.m_Entity:add_component(RigidBody(this.m_Velocity))

	-- Start the LifeTimer
	this.m_LifeTimer:start()

	setmetatable(this, self)
	return this
end

function Rain:LifeOver()
	return self.m_LifeTimer:elapsed_ms() > self.m_LifeTime
end

function Rain:Update(dt)
	if not self.m_Entity then 
		return 
	end

	local transform = self.m_Entity:get_component(Transform)
	local rigid_body = self.m_Entity:get_component(RigidBody)

	transform.position = transform.position + (rigid_body.maxVelocity * dt)
end

function Rain:Finish()
	
	if not self.m_Entity then 
		return 
	end

	local rigid_body = self.m_Entity:get_component(RigidBody)
	local animation = self.m_Entity:get_component(Animation)

	if not self.m_bStartFinish then 
		rigid_body.maxVelocity = vec2(0, 0) -- stop moving
		animation.num_frames = self.m_NumFrames
		self.m_bStartFinish = true
	else 
		-- Reset the rain to the begining position
		if animation.current_frame == animation.num_frames - 1 then 
			animation:reset()
			animation.num_frames = 0 
			local transform = self.m_Entity:get_component(Transform)
			local sprite = self.m_Entity:get_component(Sprite)
			sprite.uvs.u = 0
			transform.position = self.m_InitialPosition 

			-- Change the velocity to a random value between the min and max
			local rain_velocity = RandomFloat(self.m_MinVelocity, self.m_MaxVelocity):get_value()
			rigid_body.maxVelocity = vec2(rain_velocity, rain_velocity)

			-- Adjust the lifetime to a new random value between the min and max
			self.m_LifeTime = RandomFloat(self.m_MinLifeTime, self.m_MaxLifeTime):get_value()

			-- Reset the life timer
			self.m_LifeTimer:stop()
			self.m_LifeTimer:start() 
			self.m_bStartFinish = false
		end
	end
end

function Rain:Destroy()
	self.m_Entity:kill()
	self.m_Entity = nil
end

RainGenerator = {}
RainGenerator.__index = RainGenerator

function RainGenerator:Create(params)
	params = params or {}
	
	local this = 
	{
		m_xOffset = params.x_offset or 32,
		m_yOffset = params.y_offset or 32,
		m_RainVelMin = params.rain_vel_min or 100,
		m_RainVelMax = params.rain_vel_max or 200,
		m_RainLifeMin = params.rain_life_min or 500,
		m_RainLifeMax = params.rain_life_max or 1500,
		m_Scale = params.scale or 1
	}

	this.m_RainTable = {}

	setmetatable(this, self)

	-- Generate the rain drops
	this:ReGenerate()

	return this 
end

function RainGenerator:Update(dt)
	for k, v in pairs(self.m_RainTable) do
		--[[	
		local transform = v.m_Entity:get_component(Transform)
		if not S2D_EntityInView(transform, v.m_Width * 6, v.m_Height * 6) then
			--print("skipping:" ..v.m_Entity:id())
			goto continue 
		end
		--]]
		v:Update(dt)

		if v:LifeOver() then
			v:Finish()
		end
		--::continue::
	end 
end

function RainGenerator:ReGenerate()
	if #self.m_RainTable == 0 then 
		local maxWidth = S2D_WindowWidth()
		local maxHeight = S2D_WindowHeight()

		local rows = maxHeight / self.m_yOffset
		local cols = maxWidth / self.m_xOffset

		-- Populate rain
		for i = 0, rows do 
			for j = 0, cols do 
				local start_x = S2D_clamp(self.m_xOffset * j, 0, maxWidth) 
				local start_y = S2D_clamp(self.m_yOffset * i, 0, maxHeight) 
				local rain_velocity = RandomFloat(self.m_RainVelMin, self.m_RainVelMax):get_value()
				local rain = Rain:Create(
					{
						position = vec2(start_x, start_y),
						velocity = vec2(rain_velocity, rain_velocity),
						life_time = RandomFloat(self.m_RainLifeMin, self.m_RainLifeMax):get_value(),
						scale = self.m_Scale,
						num_frames = 1
					}
				)
				table.insert(self.m_RainTable, rain)
			end
		end
	end
end

function RainGenerator:Destroy()
	for k, v in ipairs(self.m_RainTable) do 
		v:Destroy()
		self.m_RainTable[k] = nil
	end
end
