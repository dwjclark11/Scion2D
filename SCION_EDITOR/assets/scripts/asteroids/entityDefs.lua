AsteroidDefs = 
{
	asteroid_big = 
	{
		group = "asteroids",
		components = 
		{
			transform = {
				position = { x = -100, y = -100},
				scale = { x = 1, y = 1},
				rotation = 0
			}, 
			sprite = {
				asset_name = "ast_1",
				width = 112,
				height = 80,
				start_x = 0, start_y = 0,
				layer = 2
			}
		},
		type = "big"
	},
	asteroid_small = 
	{
		group = "asteroids",
		components = 
		{
			transform = {
				position = { x = -100, y = -100},
				scale = { x = 1, y = 1},
				rotation = 0
			}, 
			sprite = {
				asset_name = "ast_2",
				width = 32,
				height = 32,
				start_x = 0, start_y = 0,
				layer = 2
			}
		},
		type = "small"
	},
}

ShipDefs = 
{
	blue_ship = 
	{
		tag = "ship",
		components = 
		{
			transform = {
				position = { x = 320, y = 240},
				scale = { x = 1, y = 1},
				rotation = 0
			}, 
			sprite = {
				asset_name = "ship",
				width = 80,
				height = 96,
				start_x = 0, start_y = 0,
				layer = 2
			}
		}
	}
}