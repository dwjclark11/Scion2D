GameState = {}
GameState.__index = GameState 

function GameState:Create(stack)
	local this = 
	{
		m_Stack = stack,
		m_bLeft = false,
		m_SceneDimmer = SceneDimmer:Create(nil)
	}
	
	local state = State("game state")
	state:set_variable_table(this)
	state:set_on_enter( function() this:OnEnter() end )
	state:set_on_exit( function() this:OnExit() end )
	state:set_on_update( function(dt) this:OnUpdate(dt) end )
	state:set_on_render( function() this:OnRender() end )
	state:set_handle_inputs( function() this:HandleInputs() end )

	setmetatable(this, self)

	this:Initialize()

	return state
end

function GameState:Initialize()
	-- Create events
	if not gCollisionEvent then 
		gCollisionEvent = CollisionEvent:Create()
	end 

	if not gTriggerSystem then 
		gTriggerSystem = TriggerSystem:Create()
	end 

	gCollisionEvent:SubscribeToEvent(gTriggerSystem)

	-- Create the player
	if not gPlayer then 
		gPlayer = Entity("player", "")

		local playerTransform = gPlayer:add_component(Transform(vec2(16, 416), vec2(1, 1), 0))

		local sprite = gPlayer:add_component(Sprite("player", 32, 32, 0, 0, 2))
		sprite:generate_uvs()

		gPlayer:add_component(Animation(6, 12, 0, false, true))

		local circleCollider = gPlayer:add_component(CircleCollider(8, vec2(8, 16)))

		local playerPhysAttr = PhysicsAttributes()
		playerPhysAttr.eType = BodyType.Dynamic
		playerPhysAttr.density = 75.0
		playerPhysAttr.friction = 1.0
		playerPhysAttr.restitution = 0.0
		playerPhysAttr.position = playerTransform.position 
		playerPhysAttr.radius = circleCollider.radius 
		playerPhysAttr.bCircle = true 
		playerPhysAttr.bFixedRotation = true 

		playerPhysAttr.objectData = (ObjectData("player", "", true, false, gPlayer:id()))

		-- Add Physics component to the player 
		gPlayer:add_component(PhysicsComp(playerPhysAttr))
	end

	if not gFollowCam then 
		gFollowCam = FollowCamera(
			FollowCamParams(
			{
				min_x = 0,
				min_y = 0,
				max_x = 640,
				max_y = 480,
				scale = 2
			}
			), gPlayer
		)
	end
end

function GameState:OnEnter()
	LoadMap(CreateTestPlatformerMap())
	Sound.play("rain", -1, 1)
	Sound.set_volume(1, 30)

	Music.play("snowfall", -1)
	Music.set_volume(50)
end

function GameState:OnExit()
	-- TODO: Destroy all entities
end

function GameState:OnUpdate(dt)
	self:UpdatePlayer(gPlayer)
	self:UpdateContacts()
	gFollowCam:update()
	self.m_SceneDimmer.m_OnUpdate()
	self.m_SceneDimmer:UpdateRainGen(dt)
end

function GameState:OnRender()
end

function GameState:HandleInputs()
	if Keyboard.just_released(KEY_BACKSPACE) then 
		self.m_Stack:pop()
		return 
	end
end

function GameState:UpdatePlayer(player)
	local physics = player:get_component(PhysicsComp)
	local velocity = physics:get_linear_velocity()
	local sprite = player:get_component(Sprite)

	-- Stop the player if we aare not pressing the button 
	physics:set_linear_velocity(vec2(0, velocity.y))

	-- Move player left or right
	if Keyboard.pressed(KEY_A) then 
		physics:set_linear_velocity(vec2(-10, velocity.y))
		bLeft = true 
		sprite.start_y = 3
	elseif Keyboard.pressed(KEY_D) then 
		physics:set_linear_velocity(vec2(10, velocity.y))
		bLeft = false 
		sprite.start_y = 2
	end

	-- Make the player Jump
	if Keyboard.just_pressed(KEY_W) then
		physics:set_linear_velocity(vec2(velocity.x, 0))
		physics:linear_impulse(vec2(velocity.x, -1250))
	end

	if velocity.y < 0 then 
		physics:set_gravity_scale(2)
	elseif velocity.y > 0 then 
		physics:set_gravity_scale(5)
	end

	-- Reset back to idle Animation
	if velocity.x == 0.0 then 
		if bLeft then 
			sprite.start_y = 1 
		else 
			sprite.start_y = 0 
		end
	end

	sprite:inspect_y()	
end

function GameState:UpdateContacts()
	local uda, udb = ContactListener.get_user_data()
	if uda and udb then 
		gCollisionEvent:EmitEvent(uda, udb)
	end 
end
