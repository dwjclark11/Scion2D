assert(MoveState, "MoveState does not exist")


CharacterStates = 
{
	move = MoveState
}

PlayerDefs = 
{
	dustin = 
	{
		tag = "player",
		group = "",
		type = "PLAYER",
		components = 
		{
			transform = {
				position = vec2(16, 416),
				scale = vec2(1, 1),
				rotation = 0.0
			},
			sprite = {
				texture = "player",
				width = 32, 
				height = 32, 
				start_x = 0, 
				start_y = 0, 
				layer = 3
			},
			animation = {
				num_frames = 4,
				frame_rate = 10,
				frame_offset = 0,
				bVertical = false,
				bLooped = true
			},
			circle_collider = {
				radius = 8,
				offset = vec2(8, 12)
			},
			physics_attributes = {
				type = BodyType.Dynamic,
				density = 75.0,
				friction = 1.0,
				restitution = 0.0,
				position = vec2(16, 416),
				radius = 8,
				bCircle = true,
				bFixedRotation = true,
				object_data = {
					tag = "player",
					group = "",
					bCollider = true,
					bTrigger = false, 
					bIsFriendly = true
				}
			}
		},
		controller = { "move" },
		default_state = "move"
	}
}