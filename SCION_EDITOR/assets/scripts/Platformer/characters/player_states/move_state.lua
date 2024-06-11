MoveState = {}
MoveState.__index = MoveState

function MoveState:Create(character)
	assert(character)
	local this = 
	{
		m_Character = character,
		m_EntityID = character.m_EntityID,
		m_Controller = character.m_Controller,
		m_JumpSteps = 10,
		m_NumJumps = 0,
		m_bAttacking = false
	}

	local state = State("move")
	state:set_variable_table(this)
	state:set_on_enter( function(...) this:OnEnter(...) end )
	state:set_on_exit( function() this:OnExit() end)
	state:set_on_update( function(dt) this:OnUpdate(dt) end)
	state:set_on_render( function() end)

	setmetatable(this, self)
	return state
end

function MoveState:OnEnter(params) end
function MoveState:OnExit() end
function MoveState:OnUpdate(dt) 
	local player = Entity(self.m_EntityID)
	local physics = player:get_component(PhysicsComp)
	local velocity = physics:get_linear_velocity()
	local sprite = player:get_component(Sprite)

	-- Stop the player if we aare not pressing the button 
	physics:set_linear_velocity(vec2(0, velocity.y))

	-- Move player left or right
	if Keyboard.pressed(KEY_A) then 
		physics:set_linear_velocity(vec2(-10, velocity.y))
		self.m_Character.m_bFacingLeft = true 
		sprite.start_y = 3
	elseif Keyboard.pressed(KEY_D) then 
		physics:set_linear_velocity(vec2(10, velocity.y))
		self.m_Character.m_bFacingLeft = false 
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
		if self.m_Character.m_bFacingLeft then 
			sprite.start_y = 1 
		else 
			sprite.start_y = 0 
		end
	end

	sprite:inspect_y()	
end
