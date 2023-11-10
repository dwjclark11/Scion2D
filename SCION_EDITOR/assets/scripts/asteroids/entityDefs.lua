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
			},
			circle_collider = {
				radius = 40
			}
		},
		type = "big",
		min_speed = -2,
		max_speed = 5,
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
			},
			circle_collider = {
				radius = 16
			}
		},
		type = "small",
		min_speed = -5,
		max_speed = 5,
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
			},
			circle_collider = {
				radius = 40
			}
		}
	}
}

ProjectileDefs =
{
	proj_1 = 
	{
		group = "projectiles",
		components = 
		{
			transform = {
				position = { x = 0, y = 0},
				scale = { x = 1, y = 1},
				rotation = 0
			}, 
			sprite = {
				asset_name = "proj_1",
				width = 32,
				height = 16,
				start_x = 0, start_y = 0,
				layer = 2
			},
			circle_collider = {
				radius = 12
			}
		}, 
		life_time = 2000,
		proj_speed = 10
	}
}