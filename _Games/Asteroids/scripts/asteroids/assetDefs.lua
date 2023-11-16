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

	-- TODO: Load other asset types
end