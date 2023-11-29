AssetDefs = 
{
	textures = 
	{
		{ name = "ship", 			path = "../_Games/Asteroids/textures/ship.png", pixel_art = true},
		{ name = "ast_1", 			path = "../_Games/Asteroids/textures/asteroid_1.png", pixel_art = true},
		{ name = "ast_2", 			path = "../_Games/Asteroids/textures/asteroid_2.png", pixel_art = true},
		{ name = "bg", 				path = "../_Games/Asteroids/textures/dark_purple_bg.png", pixel_art = true},
		{ name = "proj_1", 			path = "../_Games/Asteroids/textures/projectile_1.png", pixel_art = true},
		{ name = "lives", 			path = "../_Games/Asteroids/textures/lives_ship.png", pixel_art = true},
		{ name = "game_over", 		path = "../_Games/Asteroids/textures/game_over.png", pixel_art = true},
		{ name = "score", 			path = "../_Games/Asteroids/textures/score.png", pixel_art = true},
		{ name = "numbers", 		path = "../_Games/Asteroids/textures/numbers.png", pixel_art = true},
		{ name = "ship_explosion", 	path = "../_Games/Asteroids/textures/ship_explosion.png", pixel_art = true},
	},
	music = 
	{
		{ name = "space", path = "../_Games/Asteroids/sounds/music/space.wav"},
	},
	sound_fx = 
	{
		{ name = "big_ast_explosion", path = "../_Games/Asteroids/sounds/soundfx/big_ast_explosion.wav"},
		{ name = "small_ast_explosion", path = "../_Games/Asteroids/sounds/soundfx/small_explosion.wav"},
		{ name = "ship_explosion", path = "../_Games/Asteroids/sounds/soundfx/explosion.wav"},
		{ name = "laser", path = "../_Games/Asteroids/sounds/soundfx/laser.mp3"},
	}
}

function LoadAssets()
	for k, v in pairs(AssetDefs.textures) do
		if not AssetManager.add_texture(v.name, v.path, v.pixel_art) then
			print("Failed to load texture [" ..v.name .."] at path [" ..v.path .."]")
		else
			print("Loaded Texture [" ..v.name .."]")
		end
	end

	for k, v in pairs(AssetDefs.music) do 
		if not AssetManager.add_music(v.name, v.path) then 
			print("Failed to load music [" ..v.name .."] at path [" ..v.path .."]")
		else
			print("Loaded music [" ..v.name .."]")
		end
	end

	for k, v in pairs(AssetDefs.sound_fx) do
		if not AssetManager.add_soundfx(v.name, v.path) then
			print("Failed to load soundfx [" ..v.name .."] at path [" ..v.path .."]")
		else
			print("Loaded soundfx [" ..v.name .."]")
		end
	end
end