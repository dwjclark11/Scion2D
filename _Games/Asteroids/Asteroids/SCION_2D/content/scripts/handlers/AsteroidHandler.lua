--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-- Maximum number of asteroids the timed spawner will keep alive. Splitting a big
-- asteroid can still go above it.
local MAX_ASTEROIDS = 40

-- Tracks all active asteroids, keyed by entity id
AsteroidHandler = S2D_Class("AsteroidHandler")

function AsteroidHandler:Init()
	self.asteroids = {}
	self.count = 0
	self.spawnTimer = Timer()
	self.spawnTimer:start()
	self.bAllowSpawn = true
end

function AsteroidHandler:AddAsteroid(asteroid)
	self.asteroids[asteroid:GetID()] = asteroid
	self.count = self.count + 1
end

function AsteroidHandler:NumAsteroids()
	return self.count
end

-- Spawns one asteroid unless the cap has been reached
function AsteroidHandler:SpawnAsteroid(def)
	if self.count >= MAX_ASTEROIDS then
		return
	end

	self:AddAsteroid(Asteroid:Create({ def = def }))
end

function AsteroidHandler:Update()
	for _, asteroid in pairs(self.asteroids) do
		asteroid:Update()
	end

	if not self.bAllowSpawn then
		return
	end

	if self.spawnTimer:elapsed_sec() > 1 then
		local val = math.random(1, 3)
		if val == 1 then
			self:SpawnAsteroid("asteroid_big")
		elseif val == 2 then
			self:SpawnAsteroid("asteroid_small")
		else
			self:SpawnAsteroid("asteroid_small")
			self:SpawnAsteroid("asteroid_big")
		end

		self.spawnTimer:restart()
	end
end

function AsteroidHandler:DestroyAsteroidByID(id)
	local asteroid = self.asteroids[id]
	if asteroid then
		asteroid:Destroy()
		self.asteroids[id] = nil
		self.count = self.count - 1
	end
end

function AsteroidHandler:GetAsteroidByID(id)
	return self.asteroids[id]
end

-- Handles score, sound, splitting, pickup drops and removal
function AsteroidHandler:DestroyAsteroid(asteroid_id)
	local asteroid = self.asteroids[asteroid_id]
	if not asteroid then
		S2D_error("Failed to get asteroid by id %d", asteroid_id)
		return
	end

	if asteroid:GetType() == "big" then
		self:CreateSmallFromBig(asteroid)
		gData:AddToScore(LARGE_ASTEROID_SCORE)
		AudioPlayer.playTrack(-1, "big_ast_explosion")
	elseif asteroid:GetType() == "small" then
		gData:AddToScore(SMALL_ASTEROID_SCORE)
		AudioPlayer.playTrack(-1, "small_explosion_2")
	end

	local dropType = GetRandomDrop()
	if dropType ~= PickupTypes.None then
		local pickup = Pickup:Create(PickupDefs[dropType])
		pickup:SetPosition(vec2(asteroid.x, asteroid.y))
		gPickupHandler:AddPickup(pickup)
	end

	self:DestroyAsteroidByID(asteroid_id)
end

function AsteroidHandler:CreateSmallFromBig(asteroid)
	for i = 1, 2 do
		local small = Asteroid:Create({ def = "asteroid_small" })
		small:SetPosition(asteroid.x, asteroid.y)
		self:AddAsteroid(small)
	end
end

function AsteroidHandler:ResetAsteroids()
	for id, asteroid in pairs(self.asteroids) do
		asteroid:Destroy()
		self.asteroids[id] = nil
	end

	self.count = 0
	self.bAllowSpawn = true
	self.spawnTimer:restart()
end

function AsteroidHandler:Disable()
	self.bAllowSpawn = false
end

gAsteroidHandler = AsteroidHandler:Create()
