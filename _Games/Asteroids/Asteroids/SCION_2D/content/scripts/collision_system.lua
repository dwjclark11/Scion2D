--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-- Holds the id and damage of an entity involved in a collision
CollisionData = S2D_Class("CollisionData")

function CollisionData:Init(params)
	self.id = params.id
	self.damage = params.damage or 10
	self.bDestroy = params.bDestroy or false
end

-- Detects and resolves collisions between projectiles, asteroids, ships and pickups
CollisionSystem = S2D_Class("CollisionSystem")

local SHIP_ASTEROID_DAMAGE = 25

function CollisionSystem:Init(params)
	params = params or {}
end

function CollisionSystem:Update()
	local deadAsteroids = {}
	local hitAsteroidIDs = {}
	local hitProjectileIDs = {}

	self:DetectProjectileHits(deadAsteroids, hitAsteroidIDs, hitProjectileIDs)

	local shipHits = {}
	local pickupHits = {}
	self:DetectShipHits(deadAsteroids, shipHits, pickupHits)

	for i = 1, #hitProjectileIDs do
		gProjectileHandler:DestroyProjectile(hitProjectileIDs[i])
	end

	for i = 1, #hitAsteroidIDs do
		gAsteroidHandler:DestroyAsteroid(hitAsteroidIDs[i])
	end

	for i = 1, #shipHits do
		gShipHandler:TakeDamage(shipHits[i])
	end

	for i = 1, #pickupHits do
		local hit = pickupHits[i]
		gPickupHandler:OnHandlePickup(hit.pickupID, hit.ship)
	end
end

-- Projectile vs asteroid. One asteroid per projectile and one projectile per asteroid
function CollisionSystem:DetectProjectileHits(deadAsteroids, hitAsteroidIDs, hitProjectileIDs)
	local asteroids = gAsteroidHandler.asteroids
	local projectiles = gProjectileHandler.projectiles

	for projectileID, projectile in pairs(projectiles) do
		local px = projectile.cx
		local py = projectile.cy
		local pr = projectile.radius

		for asteroidID, asteroid in pairs(asteroids) do
			if not deadAsteroids[asteroidID] then
				local dx = px - asteroid.cx
				local dy = py - asteroid.cy
				local radSum = pr + asteroid.radius

				if (dx * dx + dy * dy) <= (radSum * radSum) then
					deadAsteroids[asteroidID] = true
					hitAsteroidIDs[#hitAsteroidIDs + 1] = asteroidID
					hitProjectileIDs[#hitProjectileIDs + 1] = projectileID
					break
				end
			end
		end
	end
end

-- Ship vs asteroid and ship vs pickup. A ship that is already colliding is skipped
function CollisionSystem:DetectShipHits(deadAsteroids, shipHits, pickupHits)
	local asteroids = gAsteroidHandler.asteroids
	local pickups = gPickupHandler.pickups

	for _, ship in pairs(gShipHandler.ships) do
		local shipEntity = Entity(ship:GetID())
		local collider = shipEntity:getComponent(CircleCollider)

		if collider and not collider.bColliding then
			local position = shipEntity:getComponent(Transform).position
			local sprite = shipEntity:getComponent(Sprite)
			local sx = position.x + sprite.width * 0.5
			local sy = position.y + sprite.height * 0.5
			local sr = collider.radius
			local bHit = false

			for asteroidID, asteroid in pairs(asteroids) do
				if not deadAsteroids[asteroidID] then
					local dx = sx - asteroid.cx
					local dy = sy - asteroid.cy
					local radSum = sr + asteroid.radius

					if (dx * dx + dy * dy) <= (radSum * radSum) then
						collider.bColliding = true
						shipHits[#shipHits + 1] = CollisionData:Create({ id = ship:GetID(), damage = SHIP_ASTEROID_DAMAGE })
						bHit = true
						break
					end
				end
			end

			if not bHit then
				for _, pickup in pairs(pickups) do
					local pickupEntity = pickup.pickup
					local pickupPos = pickupEntity:getComponent(Transform).position
					local pickupSprite = pickupEntity:getComponent(Sprite)
					local pickupCollider = pickupEntity:getComponent(CircleCollider)

					local dx = sx - (pickupPos.x + pickupSprite.width * 0.5)
					local dy = sy - (pickupPos.y + pickupSprite.height * 0.5)
					local radSum = sr + pickupCollider.radius

					if (dx * dx + dy * dy) <= (radSum * radSum) then
						pickupHits[#pickupHits + 1] = { pickupID = pickup:GetID(), ship = ship }
					end
				end
			end
		end
	end
end
