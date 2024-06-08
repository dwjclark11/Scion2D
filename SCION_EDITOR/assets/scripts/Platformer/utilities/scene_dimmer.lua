SceneDimmer = {} 
SceneDimmer.__index = SceneDimmer

function SceneDimmer:Create(params)
	params = params or {}
	local this = 
	{
		m_RainGenerator = params.rainGenerator or RainGenerator:Create(nil),
		m_Timer = Timer(),
		m_DimmerEntity = Entity("dimmer", ""),
		m_FlashTimes = params.flashTimes or {},
		m_DimColor = params.dimColor or Color(0, 0, 0, 100),
		m_LightColor = params.lightColor or Color(255, 255, 255, 25),
	}

	this.m_OnUpdate = params.onUpdate or function() this:Update() end 
	this.m_Timer:start() 

	this.m_DimmerEntity:add_component(Transform(vec2(0, 0), vec2(WindowWidth() / 16, WindowHeight() / 16), 0))

	local darkSprite = this.m_DimmerEntity:add_component(Sprite("white_box", 16, 16, 0, 0, 4))
	darkSprite.color = Color(0, 0, 0, 100)
	darkSprite:generate_uvs()

	setmetatable(this, self)
	return this 
end

function SceneDimmer:Update()
	local darkSprite = self.m_DimmerEntity:get_component(Sprite)

	if self.m_Timer:elapsed_ms() > 3000 then 
		darkSprite.color = self.m_LightColor
	end
	
	if self.m_Timer:elapsed_ms() > 3100 and self.m_Timer:elapsed_ms() < 3200 then 
		darkSprite.color = self.m_DimColor
	elseif self.m_Timer:elapsed_ms() > 3200 and self.m_Timer:elapsed_ms() < 3400 then 
		darkSprite.color = self.m_LightColor
	elseif self.m_Timer:elapsed_ms() > 3400 then 
		darkSprite.color = self.m_DimColor
		self.m_Timer:stop()
		self.m_Timer:start()
	end
end

function SceneDimmer:UpdateRainGen(dt)
	if self.m_RainGenerator then 
		self.m_RainGenerator:Update(dt)
	end
end