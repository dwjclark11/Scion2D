--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

--[[
Projectile
----------
Represents a projectile entity (e.g., laser, bullet, missile) fired by ships. 
Handles initialization from a projectile definition, movement, lifetime tracking, 
and destruction when expired.

Responsibilities:
- Initialize projectile properties from definitions
- Move the projectile each frame in a given direction
- Track projectile lifetime with a timer
- Destroy itself when its lifetime is up
]]
Projectile = S2D_Class("Projectile")

-- Initialize a projectile instance
-- params: table containing { def, dir, start_pos, rotation }
function Projectile:Init(params)
	params = params or {}
	
	self.def = params.def				-- Name of the projectile definition
	self.dir = params.dir				-- Movement direction vector
	self.startPos = params.start_pos	-- Starting world position
	self.rotation = params.rotation		-- Starting rotation angle
	
	-- Load projectile definition
	local projectile = ProjectileDefs[self.def]
	assert(projectile)

	-- Create entity from definition
	self.entityID = LoadEntity(projectile)

	-- Apply properties from definition
	self.projectileLifetime = projectile.life_time or 2000	-- ms before despawn
	self.speed = projectile.proj_speed or 10				-- Movement speed
	self.lifeTimer = Timer()								-- Lifetime tracker

	-- Set transform on entity
	local projectEnt = Entity(self.entityID)
	local transform = projectEnt:getComponent(Transform)
	transform.position = self.startPos
	transform.rotation = self.rotation
	
	-- Set projectile color 
	if params.color then 
		local sprite = projectEnt:getComponent(Sprite)
		sprite.color = params.color
	end
		
	-- Start lifetime countdown
	self.lifeTimer:start()
end

-- Update projectile each frame
-- Moves in its direction at its speed
function Projectile:Update()
	local projectEnt = Entity(self.entityID)
	local transform = projectEnt:getComponent(Transform)
	transform.position = transform.position + self.dir * self.speed
	
	-- If the projectile is outside the window boundaries, return false. 
	-- This will allow any handler to destroy this projectile if desired
	if transform.position.x < 0 or transform.position.x > S2D_WindowWidth() or 
		transform.position.y < 0 or transform.position.y > S2D_WindowHeight() then
		return false
	end
	
	return true
end

-- Get the unique entity ID of this projectile
function Projectile:GetID() 
	return self.entityID
end

-- Check if projectile lifetime has expired
-- Returns true if the projectile should be destroyed
function Projectile:TimesUp()
	return self.lifeTimer:elapsed_ms() >= self.projectileLifetime
end

-- Destroy the projectile entity
-- Removes it from the game world
function Projectile:Destroy()
	local projectile = Entity(self.entityID)
	projectile:destroy()
end
