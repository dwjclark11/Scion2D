MoveState = {}
MoveState.__index = MoveState

function MoveState:Create(character)
	assert(character)
	local this = 
	{
		m_Character = character,
		m_EntityID = character.m_EntityID,
		m_Controller = character.m_Controller,
		m_JumpSteps = 0,
		m_MaxJumpSteps = 10,
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

	local speed = 600 * dt 
	local jump_pwr = 1800 * dt 
	local bMoved = false 

	if velocity.y < 0 then 
		physics:set_gravity_scale(2)
	elseif velocity.y > 0 then 
		physics:set_gravity_scale(10)
	end

	-- Stop the player if we aare not pressing the button 
	physics:set_linear_velocity(vec2(0, velocity.y))

	if Keyboard.just_pressed(KEY_ENTER) then 
		self.m_bAttacking = true 
		self:ShootProjectile()
	elseif Keyboard.pressed(KEY_ENTER) then 
		-- TODO: Charge for stronger projectile 
		self.m_bAttacking = true 
	elseif Keyboard.just_released(KEY_ENTER) then 
		-- TODO: If charged, shoot bigger projectile 
		self.m_bAttacking = false 
	end 

	-- Move player left or right
	if Keyboard.pressed(KEY_A) then 
		physics:set_linear_velocity(vec2(-10, velocity.y))
		self.m_Character.m_bFacingLeft = true 

		if self.m_bAttacking then 
			sprite.start_y = 9
		else 
			sprite.start_y = 3
		end 
		bMoved = true 
	elseif Keyboard.pressed(KEY_D) then 
		physics:set_linear_velocity(vec2(10, velocity.y))
		self.m_Character.m_bFacingLeft = false 
		
		if self.m_bAttacking then 
			sprite.start_y = 8
		else 
			sprite.start_y = 2
		end 
		bMoved = true 
	end

	-- Make the player Jump
	if Keyboard.pressed(KEY_W) then
		if self.m_JumpSteps < self.m_MaxJumpSteps then 
			local upVelocity = (self.m_MaxJumpSteps - self.m_JumpSteps) * -jump_pwr
			physics:linear_impulse(vec2(velocity.x, upVelocity))
			self.m_JumpSteps = self.m_JumpSteps + 1
			self.m_Character.m_bGrounded = false 
			bMoved = true 
		elseif self.m_NumJumps > 0 and self.m_NumJumps < 2 then 
			physics:set_gravity_scale(2)
			physics:linear_impulse(vec2(velocity.x, self.m_MaxJumpSteps * -jump_pwr * 6))
			self.m_NumJumps = 2
		end
	end

	if Keyboard.just_released(KEY_W) then 
		physics:set_linear_velocity(vec2(velocity.x, 0))
		self.m_JumpSteps = self.m_MaxJumpSteps 
		if self.m_NumJumps == 0 then 
			self.m_NumJumps = 1
		end 
	end 

	-- Reset back to idle or standing shot Animation
	if velocity.x == 0.0 or not bMoved then 
		if self.m_Character.m_bFacingLeft then 
			if self.m_bAttacking then 
				sprite.start_y = 11 
			else
				sprite.start_y = 1
			end 
		else
			if self.m_bAttacking then
				sprite.start_y = 10 
			else
				sprite.start_y = 0 
			end 
		end
	end

	if math.floor(velocity.y) == 0 and self.m_Character.m_bGrounded then 
		self.m_JumpSteps = 0 
		self.m_NumJumps = 0 
		local vel = physics:get_linear_velocity()
		physics:set_linear_velocity(vec2(vel.x, 0))
	else 
		if self.m_Character.m_bFacingLeft then 
			if self.m_bAttacking then 
				sprite.start_y = 7 
			else 
				sprite.start_y = 5
			end
		else
			if self.m_bAttacking then 
				sprite.start_y = 6
			else 
				sprite.start_y = 4
			end
		end 
	end

	sprite:inspect_y()	
end

function MoveState:ShootProjectile()
	local player = Entity(self.m_EntityID)
	local transform = player:get_component(Transform)

	local projDir = 1 
	local position = transform.position 
	if self.m_Character.m_bFacingLeft then 
		projDir = -1 
		position = position + vec2(8, 16)
	else 
		position = position + vec2(24, 16)
	end

	AddProjectile(Projectile:Create(
		{
			def = "regular_shot",
			dir = projDir,
			start_pos = position,
			life_time = 1000
		}
	))
	-- TODO: Add Sound
	-- Sound.play("shot", 0, -1) -- Example
end