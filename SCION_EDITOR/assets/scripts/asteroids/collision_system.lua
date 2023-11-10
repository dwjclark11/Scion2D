CollisionSystem = {}
CollisionSystem.__index = CollisionSystem

function CollisionSystem:Create()
	local this = 
	{
		-- TODO: Add variables as needed
	}

	setmetatable(this, self)
	return this
end

function CollisionSystem:Update()
	self:UpdateCircleCollision()
	-- TODO: Add box collision
end

function CollisionSystem:UpdateCircleCollision()
	local entities = Registry.get_entities(CircleCollider)
	local entitiesToDestroy = {}
	entities:for_each(
		function(entity_a)
			local group_a = entity_a:group()
			local name_a = entity_a:name()
			local collider_a = entity_a:get_component(CircleCollider)
				
			entities:for_each(
				function(entity_b)
					if entity_a:id() == entity_b:id() then
						goto continue
					end

					local group_b = entity_b:group()

					if group_a == group_b then
						goto continue
					end

					local name_b = entity_b:name()
					local collider_b = entity_b:get_component(CircleCollider)
					
					if collider_a.bColliding or collider_b.bColliding then
						goto continue
					end
					
					if self:Intersect(entity_a, entity_b) then 
						if group_a == "projectiles" and group_b == "asteroids" then 
							collider_a.bColliding = true 
							collider_b.bColliding = true
							table.insert(entitiesToDestroy, entity_b:id())
						elseif group_b == "projectiles" and group_a == "asteroids" then 
							collider_a.bColliding = true 
							collider_b.bColliding = true
							table.insert(entitiesToDestroy, entity_a:id())
						elseif name_a == "ship" and group_b == "asteroids" then 
							collider_a.bColliding = true 
							table.insert(entitiesToDestroy, entity_a:id())
						elseif name_b == "ship" and group_a == "asteroids" then 
							collider_b.bColliding = true 
							table.insert(entitiesToDestroy, entity_b:id())
						end
					end

					::continue::
				end
			)
		end
	)

	for k, v in pairs(entitiesToDestroy) do 
		local entity = Entity(v)
		if entity:group() == "asteroids" then 
			RemoveAsteroid(entity:id())
		elseif entity:name() == "ship" then 
			gData:RemoveLife()
		end
	end
end

function CollisionSystem:GetCenter(entity)
	local transform = entity:get_component(Transform)
	local sprite = entity:get_component(Sprite)

	local centerSprite = vec2(sprite.width/2, sprite.height/2)
	local center = transform.position + centerSprite
	return center
end

function CollisionSystem:Intersect(entity_a, entity_b)
	
	local a_center = self:GetCenter(entity_a)
	local b_center = self:GetCenter(entity_b)

	local difference = a_center - b_center

	-- Calculate the distance squared
	local distanceSq = difference:lengthSq()

	-- Get the circle collider for the radius
	local circle_a = entity_a:get_component(CircleCollider)
	local circle_b = entity_b:get_component(CircleCollider)

	local radSum = circle_a.radius + circle_b.radius
	local radSqr = radSum * radSum
	
	return distanceSq <= radSqr
end