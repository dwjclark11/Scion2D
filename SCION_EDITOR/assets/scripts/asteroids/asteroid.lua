Asteroid = {}
Asteroid.__index = Asteroid 

function Asteroid:Create(def)
	local asteroid = AsteroidDefs[def]
	assert(asteroid, "Asteroid def does not exist!")

	local this = 
	{
		m_Def = def, 
		m_Type = asteroid.type,
		m_EntityID = -1,
		m_RotationDir = math.random(-1, 1),
		m_MinSpeed = asteroid.min_speed,
		m_MaxSpeed = asteroid.max_speed,
		m_Velocity = GetRandomVelocity(asteroid.min_speed, asteroid.max_speed),
		m_RotationSpeed = math.random(1, 4),
	}

	this.m_EntityID = LoadEntity(asteroid)
	local entity = Entity(this.m_EntityID)
	local transform = entity:get_component(Transform)
	transform.position = GetRandomPosition()

	setmetatable(this, self)
	return this
end

function Asteroid:Update()
	local entity = Entity(self.m_EntityID)
	local transform = entity:get_component(Transform)
	local sprite = entity:get_component(Sprite)

	transform.position = transform.position + self.m_Velocity
	transform.rotation = transform.rotation + (self.m_RotationSpeed * self.m_RotationDir)

	CheckPos(transform.position, sprite.width, sprite.height)
end