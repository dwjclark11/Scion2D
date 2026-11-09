--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-- Asteroid entity. Position, size and radius are tracked in Lua numbers so
-- the collision system never has to read components.
Asteroid = S2D_Class("Asteroid")

-- params: { def = "asteroid_big" | "asteroid_small" }
function Asteroid:Init(params)
	local def = params.def
	local asteroid = AsteroidDefs[def]

	assert(asteroid, "Asteroid def does not exist!")

	local sprite = asteroid.components.sprite
	local velocity = GetRandomVelocity(asteroid.min_speed, asteroid.max_speed)

	self.def = def
	self.type = asteroid.type
	self.minSpeed = asteroid.min_speed
	self.maxSpeed = asteroid.max_speed
	self.rotationDir = math.random(-1, 1)
	self.rotationSpeed = math.random(1, 4)
	self.rotation = 0
	self.vx = velocity.x
	self.vy = velocity.y
	self.width = sprite.width
	self.height = sprite.height
	self.halfWidth = sprite.width * 0.5
	self.halfHeight = sprite.height * 0.5
	self.radius = asteroid.components.circle_collider.radius

	self.entityID = LoadEntity(asteroid)
	self.entity = Entity(self.entityID)

	local position = GetRandomPosition()
	self.x = position.x
	self.y = position.y
	self.cx = self.x + self.halfWidth
	self.cy = self.y + self.halfHeight
	self.entity:getComponent(Transform).position = position
end

function Asteroid:Update()
	local w = self.width
	local h = self.height
	local x = self.x + self.vx
	local y = self.y + self.vy

	if x + w < 0 then
		x = x + WINDOW_WIDTH + w
	elseif x > WINDOW_WIDTH + w then
		x = -w
	end

	if y + h < 0 then
		y = y + WINDOW_HEIGHT + h
	elseif y > WINDOW_HEIGHT + h then
		y = -h
	end

	self.x = x
	self.y = y
	self.cx = x + self.halfWidth
	self.cy = y + self.halfHeight
	self.rotation = self.rotation + (self.rotationSpeed * self.rotationDir)

	local transform = self.entity:getComponent(Transform)
	local position = transform.position
	position.x = x
	position.y = y
	transform.rotation = self.rotation
end

function Asteroid:SetPosition(x, y)
	self.x = x
	self.y = y
	self.cx = x + self.halfWidth
	self.cy = y + self.halfHeight

	local position = self.entity:getComponent(Transform).position
	position.x = x
	position.y = y
end

function Asteroid:GetID()
	return self.entityID
end

function Asteroid:GetType()
	return self.type
end

function Asteroid:Destroy()
	self.entity:destroy()
end
