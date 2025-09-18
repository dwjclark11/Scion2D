--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-------------------------------------------------------------------
-- @class CollisionData
-- @brief Represents information about a collision event.
-- Stores entity ID, damage amount, and destruction flag.
-------------------------------------------------------------------
CollisionData = S2D_Class("CollisionData")

-------------------------------------------------------------------
-- @brief Initializes a new CollisionData object.
-- @param params table Table containing collision data parameters:
--   - id (integer) Entity ID involved in the collision.
--   - damage (integer) Optional damage value (default = 10).
--   - bDestroy (boolean) Whether entity should be destroyed (default = false).
-------------------------------------------------------------------
function CollisionData:Init(params)
	self.id = params.id
	self.damage = params.damage or 10
	self.bDestroy = params.bDestroy or false
end

-------------------------------------------------------------------
-- @class CollisionSystem
-- @brief Manages collision detection and resolution.
-- Handles asteroid, projectile, ship, and pickup interactions.
-------------------------------------------------------------------
CollisionSystem = S2D_Class("CollisionSystem")


-------------------------------------------------------------------
-- @brief Initializes the collision system.
-- @param params table Optional parameters (currently unused).
-------------------------------------------------------------------
function CollisionSystem:Init(params)
	params = params or {}
	
	self.collisionEventDispatcher = EventDispatcher(DispatcherType.Lua)
	self.collisionHandler = LuaEventHandler( function(event) self:HandleEvent(event) end )
	self.pickupHandler = LuaEventHandler( function (event) self:HandlePickups(event) end )
	self.collisionEventDispatcher:addHandler(self.collisionHandler, LuaEvent)
	self.collisionEventDispatcher:addHandler(self.pickupHandler, LuaEvent)
	self.entitiesToDestroy = {}
end

-------------------------------------------------------------------
-- @brief Updates the collision system.
-- Runs circle collision detection for all relevant entities.
-------------------------------------------------------------------
function CollisionSystem:Update()
	self:UpdateCircleCollision()
end

-------------------------------------------------------------------
-- @brief Detects and processes circle collider collisions.
-- Iterates through entities, checks intersections, and dispatches events.
-------------------------------------------------------------------
function CollisionSystem:UpdateCircleCollision()
	local reg = Registry()
	local entities = reg:getEntities( CircleCollider )
	self.entitiesToDestroy = {}
	entities:for_each(
		function(entity_a)	
			entities:for_each(
				function(entity_b)
					if entity_a:id() == entity_b:id() then
						goto continue
					end
			
					if self:Intersect(entity_a, entity_b) then 
						self.collisionEventDispatcher:emitEvent( LuaEvent( { entityA = entity_a:id(), entityB = entity_b:id() } ))
					end
					
					::continue::
				end
			)
		end
	)
		
	for k, v in pairs(self.entitiesToDestroy) do 
		local entity = Entity(v.id)
		if entity:group() == "asteroids" then 
			gAsteroidHandler:DestroyAsteroid(entity:id())
		elseif entity:group() == "projectiles" then 
			gProjectileHandler:DestroyProjectile(entity:id())
		elseif entity:name() == "PlayerShip" then 
			gShipHandler:TakeDamage(v)
		end
	end
	
	-- Handle enqueued events
	self.collisionEventDispatcher:update()
end

-------------------------------------------------------------------
-- @brief Gets the center position of an entity for collision checks.
-- @param entity Entity The entity whose center is calculated.
-- @return vec2 The center position of the entity.
-------------------------------------------------------------------
function CollisionSystem:GetCenter(entity)
	local transform = entity:getComponent(Transform)
	local sprite = entity:getComponent(Sprite)

	local centerSprite = vec2(sprite.width / 2, sprite.height / 2)
	local center = transform.position + centerSprite
	return center
end

-------------------------------------------------------------------
-- @brief Checks if two entities intersect using circle colliders.
-- @param entity_a Entity First entity.
-- @param entity_b Entity Second entity.
-- @return boolean True if entities intersect, false otherwise.
-------------------------------------------------------------------
function CollisionSystem:Intersect(entity_a, entity_b)
	
	local a_center = self:GetCenter(entity_a)
	local b_center = self:GetCenter(entity_b)

	local difference = a_center - b_center

	-- Calculate the distance squared
	local distanceSq = difference:lengthSq()

	-- Get the circle collider for the radius
	local circle_a = entity_a:getComponent(CircleCollider)
	local circle_b = entity_b:getComponent(CircleCollider)

	local radSum = circle_a.radius + circle_b.radius
	local radSqr = radSum * radSum
	
	return distanceSq <= radSqr
end

-------------------------------------------------------------------
-- @brief Handles collision events between entities.
-- @param event LuaEvent The collision event containing entity IDs.
-------------------------------------------------------------------
function CollisionSystem:HandleEvent(event)
	
	local entityA = Entity(event.data.entityA)
	local entityB = Entity(event.data.entityB)
	local group_a = entityA:group()
	local group_b = entityB:group()
	
	if group_a == group_b then
		return
	end
	
	local collider_a = entityA:getComponent(CircleCollider)
	local collider_b = entityB:getComponent(CircleCollider)
	
	if collider_a.bColliding or collider_b.bColliding then
		return
	end
	
	local name_a = entityA:name()
	local name_b = entityB:name()
	
	if group_a == "projectiles" and group_b == "asteroids" then 
		collider_a.bColliding = true 
		collider_b.bColliding = true
		S2D_InsertUnique(self.entitiesToDestroy, 
			CollisionData:Create({ id = entityB:id(), bDestroy = true })
		)
		S2D_InsertUnique(self.entitiesToDestroy, 
			CollisionData:Create({ id = entityA:id(), bDestroy = true })
		)
	elseif group_b == "projectiles" and group_a == "asteroids" then 
		collider_a.bColliding = true 
		collider_b.bColliding = true
		S2D_InsertUnique(self.entitiesToDestroy, 
			CollisionData:Create({ id = entityA:id(), bDestroy = true })
		)
		S2D_InsertUnique(self.entitiesToDestroy, 
			CollisionData:Create({ id = entityB:id(), bDestroy = true })
		)
	elseif name_a == "PlayerShip" and group_b == "asteroids" then 
		collider_a.bColliding = true 
		S2D_InsertUnique(self.entitiesToDestroy, 
			CollisionData:Create({ id = entityA:id(), damage = 25 })
		)
	elseif name_b == "PlayerShip" and group_a == "asteroids" then 
		collider_b.bColliding = true 
		S2D_InsertUnique(self.entitiesToDestroy, 
			CollisionData:Create({ id = entityB:id(), damage = 25 })
		)
	elseif name_a == "PlayerShip" and group_b == "pickups" then 
		self.collisionEventDispatcher:enqueueEvent( 
			LuaEvent( 
				{ bIsPickup = true, shipID = entityA:id(), pickupID = entityB:id() } 
			)
		)
	elseif name_b == "PlayerShip" and group_a == "pickups" then 
		self.collisionEventDispatcher:enqueueEvent( 
			LuaEvent( 
				{ bIsPickup = true, shipID = entityB:id(), pickupID = entityA:id() } 
			)
		)
	end
end

-------------------------------------------------------------------
-- @brief Handles pickup collision events.
-- @param event LuaEvent Event containing ship and pickup IDs.
-------------------------------------------------------------------
function CollisionSystem:HandlePickups(event)
	if not event.data.bIsPickup then 
		return 
	end 
	
	local ship = gShipHandler:GetShipByID(event.data.shipID)
	if not ship then 
		S2D_warn("Failed to handle pickup. Ship is not valid")
		return 
	end 
	
	gPickupHandler:OnHandlePickup(event.data.pickupID, ship)
end

