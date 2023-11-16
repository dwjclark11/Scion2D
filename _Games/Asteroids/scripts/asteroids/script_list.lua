script_list = 
{
	"../_Games/Asteroids/scripts/asteroids/utilities.lua",
	"../_Games/Asteroids/scripts/asteroids/game_data.lua",
	"../_Games/Asteroids/scripts/asteroids/assetDefs.lua",
	"../_Games/Asteroids/scripts/asteroids/entityDefs.lua",
	"../_Games/Asteroids/scripts/asteroids/projectile.lua",
	"../_Games/Asteroids/scripts/asteroids/ship.lua",
	"../_Games/Asteroids/scripts/asteroids/asteroid.lua",
	"../_Games/Asteroids/scripts/asteroids/hud.lua",
	"../_Games/Asteroids/scripts/asteroids/collision_system.lua",
}

function LoadScripts()
	for k, v in ipairs(script_list) do
		if not run_script(v) then
			print("Failed to load [" ..v .."]")
		end
	end
end