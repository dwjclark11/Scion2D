-- Main Lua Script!
run_script("../_Games/Asteroids/scripts/asteroids/script_list.lua")
run_script("../_Games/Asteroids/scripts/asteroids/main.lua")

Music.play("space")

main = {
	[1] = {
		update = function()
			if bAsteroidsLoaded then
				RunAsteroids()		
			end

			if Keyboard.just_pressed(KEY_C) then
				Music.stop()
			end

		end
	},
	[2] = {
		render = function()
			
		end
	},
}