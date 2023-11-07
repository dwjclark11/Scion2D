-- Main Lua Script!
run_script("assets/scripts/asteroids/entityDefs.lua")
run_script("assets/scripts/asteroids/assetDefs.lua")
run_script("assets/scripts/asteroids/utilities.lua")
run_script("assets/scripts/asteroids/ship.lua")
run_script("assets/scripts/asteroids/asteroid.lua")
run_script("assets/scripts/asteroids/collision_system.lua")

math.randomseed(os.time())
LoadAssets()
LoadBackground()

local entity = LoadEntity(ShipDefs["blue_ship"])
gShip = Ship:Create({id = entity})
gCollisionSystem = CollisionSystem:Create()

main = {
	[1] = {
		update = function()
			gShip:UpdateShip()
			UpdateAsteroids()
			gCollisionSystem:Update()
			SpawnAsteroid()
		end
	},
	[2] = {
		render = function()
			
		end
	},
}