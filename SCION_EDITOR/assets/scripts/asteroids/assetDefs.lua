AssetDefs = 
{
	textures = 
	{
		{ name = "ship", path = "assets/textures/ship.png", pixel_art = true},
		{ name = "ast_1", path = "assets/textures/asteroid_1.png", pixel_art = true},
		{ name = "ast_2", path = "assets/textures/asteroid_2.png", pixel_art = true},
		{ name = "bg", path = "assets/textures/dark_purple_bg.png", pixel_art = true},
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