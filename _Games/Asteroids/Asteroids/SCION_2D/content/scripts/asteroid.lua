--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-------------------------------------------------------------------
-- @class Asteroid
-- @brief Represents an asteroid entity in the game world.
-- Handles initialization, movement, rotation, and screen wrapping.
-------------------------------------------------------------------
Asteroid = S2D_Class("Asteroid")

-------------------------------------------------------------------
-- @brief Initializes an asteroid from its definition.
-- @param params table Table containing asteroid spawn parameters.
--   - def (integer) Asteroid definition key used to fetch from AsteroidDefs.
-------------------------------------------------------------------
function Asteroid:Init(params)
	local def = params.def
	local asteroid = AsteroidDefs[def]

	assert(asteroid, "Asteroid def does not exist!")
	
	self.def = def
	self.type = asteroid.type
	self.entityID = -1
	self.rotationDir = math.random(-1, 1)
	self.minSpeed = asteroid.min_speed
	self.maxSpeed = asteroid.max_speed
	self.velocity = GetRandomVelocity(asteroid.min_speed, asteroid.max_speed)
	self.rotationSpeed = math.random(1, 4)


	self.entityID = LoadEntity(asteroid)
	local entity = Entity(self.entityID)
	local transform = entity:getComponent(Transform)
	transform.position = GetRandomPosition()
end

-------------------------------------------------------------------
-- @brief Updates asteroid position and rotation each frame.
-- Moves asteroid, applies rotation, and wraps around screen edges.
-------------------------------------------------------------------
function Asteroid:Update()
	local entity = Entity(self.entityID)
	local transform = entity:getComponent(Transform)
	local sprite = entity:getComponent(Sprite)

	transform.position = transform.position + self.velocity
	transform.rotation = transform.rotation + (self.rotationSpeed * self.rotationDir)

	CheckPos(transform.position, sprite.width, sprite.height)
end

-------------------------------------------------------------------
-- @brief Gets the entity ID of the asteroid.
-- @return integer Entity ID of the asteroid.
-------------------------------------------------------------------
function Asteroid:GetID()
	return self.entityID
end

-------------------------------------------------------------------
-- @brief Gets the asteroid type.
-- @return integer Type identifier of the asteroid.
-------------------------------------------------------------------
function Asteroid:GetType() 
	return self.type
end
