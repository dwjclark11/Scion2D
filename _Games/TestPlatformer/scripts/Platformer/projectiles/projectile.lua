Projectile = {} 
Projectile.__index = Projectile 

function Projectile:Create(params)
	local this = 
	{
		m_Def = params.def, -- Name of the definition in ProjectileDefs
		m_Dir = params.dir,
		m_StartPos = params.start_pos,
		m_Rotation = params.rotation or 0,
		m_EntityID = -1,
		m_Lifetime = params.life_time or 500,
		m_LifeTimer = Timer(),
		m_Speed = params.speed or 1000,
	}

	local projectileDef = ProjectileDefs[this.m_Def]
	assert(projectileDef, "Failed to get projectile def [%s]", this.m_Def)

	projectileDef.components.physics_attributes.position = this.m_StartPos

	this.m_EntityID = LoadEntity(projectileDef)

	local projEnt = Entity(this.m_EntityID)
	local transform = projEnt:get_component(Transform)
	transform.position = this.m_StartPos 
	transform.rotation = this.m_Rotation 

	this.m_LifeTimer:start()

	setmetatable(this, self)
	return this
end

function Projectile:Update(dt)
	local projectile = Entity(self.m_EntityID)
	local physics = projectile:get_component(PhysicsComp)

	physics:set_linear_velocity (vec2(self.m_Dir * self.m_Speed * dt, 0))
end

function Projectile:TimesUp()
	return self.m_LifeTimer:elapsed_ms() >= self.m_Lifetime
end

function Projectile:Destroy()
	Entity(self.m_EntityID):kill()
	S2D_warn("Killed projectile with id: %d", self.m_EntityID)
end

