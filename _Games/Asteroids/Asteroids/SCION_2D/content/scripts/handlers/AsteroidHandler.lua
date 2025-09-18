--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-----------------------------------------------------------------------------------------------------------------
-- @class AsteroidHandler
-- @brief Manages all asteroid-related behavior including spawning,
-- updating, destruction, and score handling. Provides a centralized
-- interface for working with asteroid entities.
-----------------------------------------------------------------------------------------------------------------
AsteroidHandler = S2D_Class("AsteroidHandler")

-----------------------------------------------------------------------------------------------------------------
--- @brief Initializes the asteroid handler.
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:Init()
	self.asteroids = {}			-- Table of active asteroids
	self.spawnTimer = Timer()	-- Timer used to control asteroid spawning 
	self.spawnTimer:start()		
	self.bAllowSpawn = true		-- Flag to enable/disable spawning
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Adds an asteroid to the internal list
--- @param asteroid Asteroid object to add
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:AddAsteroid(asteroid)
	table.insert(self.asteroids, asteroid)
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Gets the number of active asteroids
--- @return integer Number of asteroids
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:NumAsteroids()
	return #self.asteroids
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Updates all asteroids and handles spawn logic
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:Update()
	 -- Update all asteroid entities
	for k, v in pairs(self.asteroids) do 
		v:Update()
	end
	
	-- Skip spawning if disabled
	if not self.bAllowSpawn then 
		return 
	end
	
	 -- Spawn new asteroids every second
	if self.spawnTimer:elapsed_sec() > 1 then 
		local val = math.random(1, 3)
		if val == 1 then 
			local asteroid = Asteroid:Create({ def = "asteroid_big" })
			self:AddAsteroid(asteroid)
		elseif val == 2 then 
			local asteroid = Asteroid:Create({ def = "asteroid_small" })
			self:AddAsteroid(asteroid)
		elseif val == 3 then 
			local asteroid1 = Asteroid:Create({ def = "asteroid_small" })
			self:AddAsteroid(asteroid1)
			local asteroid2 = Asteroid:Create({ def = "asteroid_big" })
			self:AddAsteroid(asteroid2)
		end
		
		self.spawnTimer:restart()
	end
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Destroys an asteroid by entity ID
--- @param id integer Entity ID of asteroid
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:DestroyAsteroidByID(id)
	for k, v in pairs(self.asteroids) do 
		if v:GetID() == id then 
			local asteroid = Entity(id)
			asteroid:destroy()				-- Remove the entity from the game
			self.asteroids[k] = nil		-- Remove from handler
			break
		end 
	end 
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Gets an asteroid by its entity ID
--- @param id integer Entity ID
--- @return Asteroid|nil Returns asteroid if found, otherwise nil
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:GetAsteroidByID(id)
	for k, v in pairs(self.asteroids) do 
		if v:GetID() == id then 
			return v
		end 
	end 
	
	return nil	
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Handles asteroid destruction, score update, and drops
--- @param asteroid_id integer Entity ID of asteroid
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:DestroyAsteroid(asteroid_id)
	local asteroid = self:GetAsteroidByID(asteroid_id) 
	if not asteroid then 
		S2D_error("Failed to get asteroid by id %d", asteroid_id)
		return 
	end
	
	 -- Score and explosion logic depending on asteroid type
	if asteroid:GetType() == "big" then 
		self:CreateSmallFromBig(asteroid)			-- Split into small asteroids
		gData:AddToScore(LARGE_ASTEROID_SCORE)		-- Add big asteroid score
		Sound.play("big_ast_explosion")
	elseif asteroid:GetType() == "small" then
		gData:AddToScore(SMALL_ASTEROID_SCORE)		-- Add small asteroid score
		Sound.play("small_explosion_2")
	end
	
	 -- Get asteroid position for potential pickup drop
	local transform = Entity(asteroid_id):getComponent(Transform)
	local position = transform.position
	
	 -- Handle random pickup drop
	local dropType = GetRandomDrop()
	if dropType ~= PickupTypes.None then 
		local pickup = Pickup:Create(PickupDefs[dropType])
		pickup:SetPosition(position)
		gPickupHandler:AddPickup(pickup)
	end
	
	 -- Finally, remove asteroid
	self:DestroyAsteroidByID(asteroid_id)
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Creates two small asteroids from a destroyed big asteroid
--- @param asteroid Asteroid object (big type)
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:CreateSmallFromBig(asteroid)
	local transform = Entity(asteroid:GetID()):getComponent(Transform)
	for i = 1, 2 do 
		local small = Asteroid:Create({ def = "asteroid_small" })
		local small_transform = Entity(small:GetID()):getComponent(Transform)
		small_transform.position = transform.position
		self:AddAsteroid(small)
	end
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Clears all asteroids and destroys their entities
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:ResetAsteroids()
	for k, v in pairs(self.asteroids) do 
		local asteroid = Entity(v:GetID())
		asteroid:destroy()
		self.asteroids[k] = nil
	end
	
	self.bAllowSpawn = true
	self.spawnTimer:restart()
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Disables spawning of new asteroids
-----------------------------------------------------------------------------------------------------------------
function AsteroidHandler:Disable()
	self.bAllowSpawn = false
end

-- Global instance of asteroid handler
gAsteroidHandler = AsteroidHandler:Create()
