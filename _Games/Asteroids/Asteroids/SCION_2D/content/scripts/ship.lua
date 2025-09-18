--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]


-----------------------------------------------------------------------------------------------------------------
-- Ship
-- @brief Represents a controllable player ship entity in the game.
-- Handles movement, rotation, shooting projectiles, taking damage,
-- explosions, respawning, invincibility frames, and weapon upgrades.
-----------------------------------------------------------------------------------------------------------------
Ship = S2D_Class("Ship")

-----------------------------------------------------------------------------------------------------------------
-- @brief Initializes the ship with parameters like speed, cooldowns, health, and shield.
-- Also creates shield entity and attaches it as a child.
-- @param params table containing ship configuration (id, speed, cooldown, etc.)
-----------------------------------------------------------------------------------------------------------------
function Ship:Init(params)
	self.entityID = params.id
	self.forwardSpeed = params.fwd_speed or 6.5			-- Base forward movement speed
	self.angularSpeed = params.ang_speed or 4			-- Rotation speed (degrees per frame)
	self.driftSpeed = params.drift_speed or 1			-- Drift speed when not accelerating
	self.driftAngle = vec2( math.cos(0), math.sin(0) )	-- Default drift vector
	self.coolDown = params.cool_down or 300				-- Weapon cooldown in ms
	
	self.weaponUpgradeTime = 20
	
	-- Timers for handling cooldowns, death, and invincibility
	self.cooldownTimer = Timer()
	self.deathTimer = Timer()
	self.invincibleTimer = Timer()
	self.damageTimer = Timer()
	self.damageFlashTimer = Timer()
	self.shieldDamageTimer = Timer()
	self.shieldDamageFlashTimer = Timer()
	self.weaponUpgradeTimer = Timer()
	
	-- State flags
	self.bExplode = false
	self.bDead = false
	self.bShieldEnabled = true
	self.bTakingDamage = false
	
	self.numLives = 3
	self.shipHealth = 100
	self.shieldHealth = 100
	self.score = 0
	self.playerNum = 0
	self.currentWeapon = 0
	self.inputManager = InputManager:Create()
	self.eventDispatcher = EventDispatcher(DispatcherType.Lua)

	-- Create shield entity and attach to ship
	local shipEntity = Entity(self.entityID)
	local shipTransform = shipEntity:getComponent(Transform)
	local shieldEntity = Entity("", "shield")
	local shieldTransform = shieldEntity:addComponent(Transform(shipTransform.position, vec2(1, 1), 0.0))
	
	local shieldSprite = shieldEntity:addComponent( Sprite( "shield", 112, 144, 0, 0, 3 ) )
	shieldSprite:generateUVs()
	shieldSprite.bHidden = false
	shipEntity:addChild(shieldEntity)	
	shieldTransform.localPosition = vec2(-10, -16)
	self.shieldID = shieldEntity:id()
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Sets the player number and configures the input manager accordingly.
-- @param num integer Player number (1 or 2)
-----------------------------------------------------------------------------------------------------------------
function Ship:SetPlayerNum(num)
	self.playerNum = num
	self.inputManager:SetPlayerNum(num)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Adds an event handler to the ship's internal event dispatcher.
-- @param eventHandler function The Lua event callback
-- @param eventType table The type of event to listen for (defaults to LuaEvent)
-----------------------------------------------------------------------------------------------------------------
function Ship:AddEventHandler(eventHandler, eventType)
	eventType = eventType or LuaEvent
	self.eventDispatcher:addHandler(eventHandler, eventType)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Returns the unique entity ID of this ship.
-- @return integer entityID
-----------------------------------------------------------------------------------------------------------------
function Ship:GetID()
	return self.entityID 
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Returns the number of remaining lives for this ship.
-- @return integer numLives
-----------------------------------------------------------------------------------------------------------------
function Ship:GetLives()
	return self.numLives
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Returns the current health of the ship.
-- @return number shipHealth
-----------------------------------------------------------------------------------------------------------------
function Ship:GetHealth()
	return self.shipHealth
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Returns the current shield health of the ship.
-- @return number shieldHealth
-----------------------------------------------------------------------------------------------------------------
function Ship:GetShieldHealth()
	return self.shieldHealth
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Returns true if the shield is currently enabled.
-- @return boolean
-----------------------------------------------------------------------------------------------------------------
function Ship:ShieldEnabled()
	return self.bShieldEnabled 
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Adds lives to the ship and emits a life event.
-- @param lives integer Number of lives to add
-----------------------------------------------------------------------------------------------------------------
function Ship:AddLives(lives)
	self.numLives = S2D_clamp(self.numLives + lives, 0, 3)
	self.eventDispatcher:emitEvent(
		LuaEvent( 
			{ 	
				type = ShipEventType.LifeEvent, 
				event = ShipLifeEvent:Create({ id = self.entityID } )
			} 
		)
	)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Heals the ship by a specified amount and emits a health event.
-- @param amount number Amount to heal (defaults to 10)
-----------------------------------------------------------------------------------------------------------------
function Ship:AddHealth( amount )
	amount = amount or 10
	self.shipHealth = self.shipHealth + amount 
	
	self.eventDispatcher:emitEvent(
			LuaEvent( 
				{ 	
					type = ShipEventType.HealthEvent, 
					event = ShipHealthEvent:Create({ id = self.entityID, amount = -amount } )
				} 
			)
		)	
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Restores shield health, enables shield if needed, and emits shield event.
-- @param amount number Amount to heal (defaults to 10)
-----------------------------------------------------------------------------------------------------------------
function Ship:AddShieldHealth( amount )
	amount = amount or 10
	self.shieldHealth = self.shieldHealth + amount 
	
	if not self.bShieldEnabled and self.shieldHealth > 0 then 
		self.bShieldEnabled = true 
		local shield = Entity(self.shieldID)
		local shieldSprite = shield:getComponent(Sprite)
		shieldSprite.bHidden = false
	end
	
	self.eventDispatcher:emitEvent(
			LuaEvent( 
				{ 	
					type = ShipEventType.ShieldEvent, 
					event = ShipShieldEvent:Create({ id = self.entityID, amount = -amount } )
				} 
			)
		)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Upgrades the ship's weapon, adjusting cooldown and setting upgrade timer.
-----------------------------------------------------------------------------------------------------------------
function Ship:UpgradeWeapon()
	Sound.play("weapon_powerup")
	self.currentWeapon = S2D_clamp(self.currentWeapon + 1, 0, 3)
	
	if self.currentWeapon == 1 then 
		self.coolDown = 100 
	elseif self.currentWeapon == 2 or self.currentWeapon == 3 then 
		self.coolDown = 0 
	end
	
	self.weaponUpgradeTimer:restart()	
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates weapon upgrade timer and resets weapon if duration elapsed.
-----------------------------------------------------------------------------------------------------------------
function Ship:UpdateWeapon()
	if self.currentWeapon == 0 then 
		return 
	end
	
	if self.weaponUpgradeTimer:elapsed_sec() >= self.weaponUpgradeTime then  
		self.weaponUpgradeTimer:stop()
		self.currentWeapon = 0 
		self.coolDown = 350
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Applies damage to the ship or shield. Returns true if ship dies.
-- @param amount number Amount of damage
-- @return boolean true if ship health <= 0
-----------------------------------------------------------------------------------------------------------------
function Ship:TakeDamage(amount)
	if self.bTakingDamage then 
		return false 
	end
	
	self.bTakingDamage = true
	
	if self.bShieldEnabled then
		self.shieldHealth = S2D_clamp(self.shieldHealth - amount, 0, 100)
		
		Sound.play("shield_hit_1")
		if self.shieldHealth <= 0 then 
			self.bShieldEnabled = false 
			self.shieldHealth = 0 
			local shield = Entity(self.shieldID)
			local shieldSprite = shield:getComponent(Sprite)
			shieldSprite.bHidden = true
		end 
		
		self.shieldDamageTimer:start()
		
		self.eventDispatcher:emitEvent(
			LuaEvent( 
				{ 	
					type = ShipEventType.ShieldEvent, 
					event = ShipShieldEvent:Create({ id = self.entityID, amount = amount } )
				} 
			)
		)
		
		-- Trigger rumble controller effect if supported
		self.inputManager:Rumble(0x7fff, 0x7fff, 1000)
		
		return false
	else
		self.shipHealth = S2D_clamp(self.shipHealth - amount, 0, 100)
		self.damageTimer:start()
		
		self.eventDispatcher:emitEvent(
			LuaEvent( 
				{ 	
					type = ShipEventType.HealthEvent, 
					event = ShipHealthEvent:Create({ id = self.entityID, amount = amount } )
				} 
			)
		)
		
		Sound.play("ship_hit_1")
		Sound.play("ship_damage_1")
		
		self.inputManager:Rumble(0xffff, 0xffff, 1000)
		
		return self.shipHealth <= 0
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Main update function called each frame to handle movement, rotation,
-- firing, ghost effects, shield rotation, and position wrapping.
-----------------------------------------------------------------------------------------------------------------
function Ship:Update()
	-- If ship is dead, no updates
	if self.bDead then 
		return 
	end
	
	-- If ship is exploding, update explosion sequence
	if self.bExplode then 
		self:UpdateExplosion()
		return
	end

	-- Get ship components
	local ship = Entity(self.entityID)
	local transform = ship:getComponent(Transform)
	local sprite = ship:getComponent(Sprite)

	-- Calculate forward vector from rotation
	local forward = vec2(
		math.cos(math.rad(transform.rotation)), -- x direction
		math.sin(math.rad(transform.rotation))  -- y direction
	)

	-- Allow speed boost when holding Right Shift
	local speedUp = 1.0
	if self.inputManager:Boost() then 
		speedUp = 1.5
	end

	-- Handle rotation (A = CCW, D = CW)
	if self.inputManager:RotateCCW() then
		transform.rotation = transform.rotation - self.angularSpeed * speedUp
	elseif self.inputManager:RotateCW() then 
		transform.rotation = transform.rotation + self.angularSpeed * speedUp
	end

	-- Handle forward thrust (W = accelerate, otherwise drift)
	if self.inputManager:MoveForward() then 
		transform.position = transform.position + forward * self.forwardSpeed * speedUp
		self.driftAngle = forward
	else
		transform.position = transform.position + self.driftAngle * self.driftSpeed
	end

	-- Handle projectile shooting
	if not self.cooldownTimer:is_running() then 
		if self.inputManager:Fire() then	
			-- Create and launch projectile
			local projectile = Projectile:Create(
				{
					def = "proj_1",
					dir = forward, 
					start_pos = vec2(
						transform.position.x + sprite.width / 2,
						transform.position.y + sprite.height / 2
					),
					rotation = transform.rotation,
					color = self:GetProjColor()
				}
			)
			self.cooldownTimer:start()
			gProjectileHandler:AddProjectile(projectile)
			Sound.play("laser")
		end
	-- Reset cooldown when elapsed
	elseif self.cooldownTimer:elapsed_ms() >= self.coolDown then 
		self.cooldownTimer:stop()
	end
	
	self:UpdateGhostEffect()
	self:UpdateWeapon()
	
	-- Check if ship needs to transport to other side of screen
	CheckPos(transform.position, sprite.width, sprite.height)
	
	if self.bShieldEnabled then 
		local shieldEntity = Entity(self.shieldID)
		local shieldTransform = shieldEntity:getComponent(Transform)
		shieldTransform.localRotation = shieldTransform.localRotation + 25.0
		if shieldTransform.localRotation >= 360 then
			shieldTransform.localRotation = 0
		end
	end
	
	ship:updateTransform()
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Handles explosion animation, lives reduction, and death timer.
-----------------------------------------------------------------------------------------------------------------
function Ship:OnShipDestroy()
	-- If lives decreased, trigger explosion
	if not self.bExplode then 
		self.bExplode = true
		self.shipHealth = 100 -- Reset health back to 100
		local ship = Entity(self.entityID)
		
		-- Set explosion animation
		local animation = ship:getComponent(Animation)
		animation.numFrames = 11 
		animation.frameRate = 15
		animation:reset()
		
		-- Set explosion sprite
		local sprite = ship:getComponent(Sprite)		
		sprite.sTextureName = "ship_explosion"
		sprite.width = 80
		sprite.height = 80
		sprite:generateUVs()

		-- Enlarge explosion scale
		local transform = ship:getComponent(Transform)
		transform.scale = vec2(2, 2)
		
		self.numLives = self.numLives - 1
		self.deathTimer:start()
		Sound.play("ship_explosion")
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates ghost/invincibility effect and flashing when taking damage.
-----------------------------------------------------------------------------------------------------------------
function Ship:UpdateGhostEffect()
	-- Handle invincibility frames (ghost effect after respawn)
	if self.invincibleTimer:is_running() then 
		local ship = Entity(self.entityID)
		
		local sprite = ship:getComponent(Sprite)
		sprite.color.a = 150 -- Semi-transparent ghost mode

		if self.invincibleTimer:elapsed_ms() > 3000 then 
			-- Reset collision after 3 seconds
			local collider = ship:getComponent(CircleCollider)
			collider.bColliding = false 

			self.invincibleTimer:stop() 
			sprite.color.a = 255
		end 
	elseif self.damageTimer:is_running() then 
		local ship = Entity(self.entityID)
		local sprite = ship:getComponent(Sprite)
		
		FlashColor(sprite, self.damageFlashTimer, 50, Color(255, 0, 0, 200), Color(255, 255, 255, 200))
		
		-- Reset damage collision after .5 second
		if self.damageTimer:elapsed_ms() > 500 then 
			self.bTakingDamage = false
			local collider = ship:getComponent(CircleCollider)
			collider.bColliding = false 
			sprite.color = Color(255, 255, 255, 255)
			self.damageTimer:stop()
			self.damageFlashTimer:stop()
		end
	elseif self.shieldDamageTimer:is_running() then 
		local ship = Entity(self.entityID)
		local sprite = ship:getComponent(Sprite)
		
		FlashColor(sprite, self.shieldDamageFlashTimer, 50, Color(0, 0, 255, 200), Color(255, 255, 255, 200))
		
		-- Reset damage collision after .5 second
		if self.shieldDamageTimer:elapsed_ms() > 500 then 
			self.bTakingDamage = false
			local collider = ship:getComponent(CircleCollider)
			collider.bColliding = false 
			sprite.color = Color(255, 255, 255, 255)
			self.shieldDamageTimer:stop()
			self.shieldDamageFlashTimer:stop()
		end
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates explosion animation sequence and resets ship post-explosion.
-----------------------------------------------------------------------------------------------------------------
function Ship:UpdateExplosion()
	if self.bExplode and self.deathTimer:elapsed_ms() > 1000 then 
		local ship = Entity(self.entityID)
		local animation = ship:getComponent(Animation)
		
		-- Check if explosion finished
		if animation.currentFrame >= animation.numFrames - 1 then
			-- Reset Animations
			animation.numFrames = 0
			animation.frameRate = 0
			animation.currentFrame = 0

			-- Reset to normal ship sprite
			local sprite = ship:getComponent(Sprite)
			sprite.sTextureName = "ship"
			sprite.width = 80
			sprite.height = 112 
			sprite:generateUVs()
			sprite.startX = 0

			-- Reset the scale -- transform
			local transform = ship:getComponent(Transform)
			transform.scale = vec2(1, 1)

			-- Clear explosion state and enable invincibility
			self.bExplode = false 
			self.deathTimer:stop()
			self.invincibleTimer:start()
			
			-- If no lives remain, hide ship permanently
			if self.numLives == 0.0 then 
				if not self.bDead then 
					sprite.bHidden = true
				end
				self.bDead = true
			else
				self.eventDispatcher:emitEvent(
					LuaEvent( 
						{ 	
							type = ShipEventType.HealthEvent, 
							event = ShipHealthEvent:Create({ id = self.entityID, amount = self.shipHealth } )
						} 
					)
				)
			end
		end
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Resets the ship to initial state, including health, shield, timers,
-- visibility, and emits appropriate events.
-----------------------------------------------------------------------------------------------------------------
function Ship:Reset()
	self.bExplode = false 
	self.bDead = false 
	self.numLives = 3
	self.shipHealth = 100
	self.shieldHealth = 100
	self.bShieldEnabled = true
	self.bTakingDamage = false
	
	-- Reset timers
	self.deathTimer:stop()
	self.invincibleTimer:stop() 
	self.cooldownTimer:stop()
	self.damageTimer:stop()
	self.damageFlashTimer:stop()
	self.shieldDamageTimer:stop()
	self.shieldDamageFlashTimer:stop()
		
	-- Reset sprite visibility and collider state 
	local ship = Entity(self.entityID)
	local shipSprite = ship:getComponent(Sprite)
	shipSprite.bHidden = false 
	
	local shield = Entity(self.shieldID)
	local shieldSprite = shield:getComponent(Sprite)
	shieldSprite.bHidden = false 
	
	local circle_collider = ship:getComponent(CircleCollider)
	circle_collider.bColliding = false
	
	self.eventDispatcher:emitEvent(
		LuaEvent( 
			{ 	
				type = ShipEventType.HealthEvent, 
				event = ShipHealthEvent:Create({ id = self.entityID, amount = self.shipHealth } )
			} 
		)
	)
	
	self.eventDispatcher:emitEvent(
		LuaEvent( 
			{ 	
				type = ShipEventType.ShieldEvent, 
				event = ShipShieldEvent:Create({ id = self.entityID } )
			} 
		)
	)
	
	self.eventDispatcher:emitEvent(
		LuaEvent( 
			{ 	
				type = ShipEventType.LifeEvent, 
				event = ShipLifeEvent:Create({ id = self.entityID } )
			} 
		)
	)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Returns the projectile color based on the current weapon.
-- @return Color
-----------------------------------------------------------------------------------------------------------------
function Ship:GetProjColor()
	if self.currentWeapon == 0 then 
		return Color(255, 255, 255, 255)
	elseif self.currentWeapon == 1 then  
		return Color(255, 255, 0, 255)
	elseif self.currentWeapon == 2 then  	
		return Color(255, 0, 0, 255)
	elseif self.currentWeapon == 3 then  
		return Color(255, 0, 0, 255)
	end 
	
	return Color(255, 255, 255, 255)
end 


