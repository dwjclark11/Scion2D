Projectile = {}
Projectile.__index = Projectile

function Projectile:Create(params)
	local this = 
	{
		m_Def = params.def,
		m_Dir = params.dir,
		m_StartPos = params.start_pos, 
		m_Rotation = params.rotation
	}
	
	local projectile = ProjectileDefs[this.m_Def]
	assert(projectile)

	this.m_EntityID = LoadEntity(projectile)

	this.m_ProjectileLifetime = projectile.life_time or 2000
	this.m_Speed = projectile.proj_speed or 10
	this.m_LifeTimer = Timer()

	local projectEnt = Entity(this.m_EntityID)
	local transform = projectEnt:get_component(Transform)
	transform.position = this.m_StartPos
	transform.rotation = this.m_Rotation
	this.m_LifeTimer:start()

	setmetatable(this, self)
	return this
end

function Projectile:Update()
	local projectEnt = Entity(self.m_EntityID)
	local transform = projectEnt:get_component(Transform)
	transform.position = transform.position + self.m_Dir * self.m_Speed
end

function Projectile:TimesUp()
	return self.m_LifeTimer:elapsed_ms() >= self.m_ProjectileLifetime
end

function Projectile:Destroy()
	local projectile = Entity(self.m_EntityID)
	projectile:kill()
end