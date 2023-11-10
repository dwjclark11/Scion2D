Ship = {}
Ship.__index = Ship

function Ship:Create(def)
	local this = 
	{
		m_EntityID = def.id,
		m_ForwardSpeed = def.fwd_speed or 10,
		m_AngularSpeed = def.ang_speed or 4,
		m_DriftSpeed = def.drift_speed or 1,
		m_DriftAngle = vec2(
			math.cos(0),
			math.sin(0)
		),
		m_CoolDown = def.cool_down or 200,

		-- Timers
		m_CooldownTimer = Timer(),
		m_DeathTimer = Timer(),
		m_InvincibleTimer = Timer(),

		m_bDead = false,
		m_NumLives = gData:NumLives(),
	}

	setmetatable(this, self)
	return this
end

function Ship:UpdateShip()
	if self.m_bDead then 
		return 
	end
	
	local ship = Entity(self.m_EntityID)
	local transform = ship:get_component(Transform)
	local sprite = ship:get_component(Sprite)

	local forward = vec2(
		math.cos(math.rad(transform.rotation)), -- x Value
		math.sin(math.rad(transform.rotation)) -- y Value
	)
	-- CCW Rotation
	if Keyboard.pressed(KEY_A) then
		transform.rotation = transform.rotation - self.m_AngularSpeed
	elseif Keyboard.pressed(KEY_D) then -- CW
		transform.rotation = transform.rotation + self.m_AngularSpeed
	end

	if Keyboard.pressed(KEY_W) then 
		transform.position = transform.position + forward * self.m_ForwardSpeed
		self.m_DriftAngle = forward
	else
		transform.position = transform.position + self.m_DriftAngle * self.m_DriftSpeed
	end

	if not self.m_CooldownTimer:is_running() then 
		if Keyboard.just_pressed(KEY_SPACE) then	
			local projectile = Projectile:Create(
				{
					def = "proj_1",
					dir = forward, 
					start_pos = vec2(
						transform.position.x + sprite.width/2,
						transform.position.y + sprite.height/2
					),
					rotation = transform.rotation
				}
			)
			self.m_CooldownTimer:start()
			AddProjectile(projectile)
		end
	elseif self.m_CooldownTimer:elapsed_ms() >= self.m_CoolDown then 
		self.m_CooldownTimer:stop()
	end

	self:CheckDeath()

	CheckPos(transform.position, sprite.width, sprite.height)
end

function Ship:CheckDeath()
	if self.m_NumLives ~= gData:NumLives() then 
		self.m_NumLives = gData:NumLives()
		self.m_InvincibleTimer:start()
	end

	if self.m_InvincibleTimer:is_running() then 
		local ship = Entity(self.m_EntityID)
		local sprite = ship:get_component(Sprite)
		sprite.color.a = 150

		if self.m_InvincibleTimer:elapsed_ms() > 3000 then 
			local collider = ship:get_component(CircleCollider)
			collider.bColliding = false 
			self.m_InvincibleTimer:stop() 
			sprite.color.a = 255
		end 
	end
end