-------------------------------------------------------------------
-- Simple Constants
-------------------------------------------------------------------
WINDOW_WIDTH = 640
WINDOW_HEIGHT = 480
SMALL_ASTEROID_SCORE = 15
LARGE_ASTEROID_SCORE = 100
-------------------------------------------------------------------

-------------------------------------------------------------------
-- Helper Functions
-------------------------------------------------------------------
--[[
	Takes in an entity definition and builds a new entity by
	adding the given components and values based on the provided 
	definition.
	Returns the newly created entity's ID
--]]
function LoadEntity(def)
	assert(def, "Def does not exist")

	local tag = ""
	if def.tag then
		tag = def.tag 
	end

	local group = ""
	if def.group then 
		group = def.group 
	end 

	local newEntity = Entity(tag, group)

	if def.components.transform then
		newEntity:add_component(
			Transform(
				vec2(
					def.components.transform.position.x,
					def.components.transform.position.y
				),
				vec2(
					def.components.transform.scale.x,
					def.components.transform.scale.y
				),
				def.components.transform.rotation
			)
		)
	end

	if def.components.sprite then
		local sprite = newEntity:add_component(
			Sprite(
				def.components.sprite.asset_name,
				def.components.sprite.width,
				def.components.sprite.height,
				def.components.sprite.start_x,
				def.components.sprite.start_y,
				def.components.sprite.layer
			)
		)
		sprite:generate_uvs()
		sprite.bHidden = def.components.sprite.bHidden or false
	end

	if def.components.circle_collider then
		newEntity:add_component(
			CircleCollider(
				def.components.circle_collider.radius
			)
		)
	end

	if def.components.animation then 
		newEntity:add_component(
			Animation(
				def.components.animation.num_frames,
				def.components.animation.frame_rate,
				def.components.animation.frame_offset,
				def.components.animation.bVertical,
				def.components.animation.bLooped
			)
		)
	end

	return newEntity:id()
end

function LoadBackground()
	for i = 0, 2 do 
		for j = 0, 3 do
			local bgTile = Entity("", "bg")
			bgTile:add_component(Transform(vec2(j * 256, i * 256), vec2(1, 1), 0))
			local sprite = bgTile:add_component(Sprite("bg", 256.0, 256.0, 0, 0, 0))
			sprite:generate_uvs()
		end
	end
end

----------------------------------------------------------------------------
-- Position = vec2, width/height = float
function CheckPos(position, width, height)
	local min_x = 0
	local min_y = 0
	local max_x = WINDOW_WIDTH
	local max_y = WINDOW_HEIGHT

	if position.x + width < min_x then 
		position.x = position.x + WINDOW_WIDTH + width
	elseif position.x > max_x + width then 
		position.x = -width
	end

	if position.y + height < min_y then 
		position.y = position.y + WINDOW_HEIGHT + height
	elseif position.y > max_y + height then 
		position.y = -height
	end
end

function GetRandomVelocity(min_speed, max_speed)
	return vec2(
		math.random(min_speed, max_speed),
		math.random(min_speed, max_speed)
	)
end

function GetRandomPosition()
	return vec2(
		math.random(WINDOW_WIDTH) + WINDOW_WIDTH,
		math.random(WINDOW_HEIGHT) + WINDOW_HEIGHT
	)
end
----------------------------------------------------------------------------

----------------------------------------------------------------------------
-- Asteroids Holder Helper Functions
----------------------------------------------------------------------------
Asteroids = {}
function AddAsteroid(asteroid)
	table.insert(Asteroids, asteroid)
end

function UpdateAsteroids()
	for k, v in pairs(Asteroids) do 
		v:Update()
	end
end

--[[
	Removes the asteroid that was hit with a projectile. 
	If the asteroid that was hit was a big asteroid, two 
	smaller asteroids will be created.
--]]
function RemoveAsteroid(asteroid_id)
	for k, v in pairs(Asteroids) do 
		if v.m_EntityID == asteroid_id then 
			-- Check the Asteroids Type
			if v.m_Type == "big" then 
				CreateSmallFromBig(v)
				gData:AddToScore(LARGE_ASTEROID_SCORE)
				Sound.play("big_ast_explosion")
			elseif v.m_Type == "small" then
				gData:AddToScore(SMALL_ASTEROID_SCORE)
				Sound.play("small_ast_explosion")
			end

			local asteroid = Entity(v.m_EntityID)
			asteroid:kill()
			Asteroids[k] = nil
		end
	end
end

--[[
	Creates two small asteroids from one big asteroid. 
	It will create the small asteroids in the same position
	as the big asteroid and then give them random velocities
--]]
function CreateSmallFromBig(asteroid)
	local transform = Entity(asteroid.m_EntityID):get_component(Transform)
	for i = 1, 2 do 
		local small = Asteroid:Create("asteroid_small")
		local small_transform = Entity(small.m_EntityID):get_component(Transform)
		small_transform.position = transform.position
		AddAsteroid(small)
	end
end

gSpawnTimer = Timer()
function SpawnAsteroid()
	if not gSpawnTimer:is_running() then
		gSpawnTimer:start()
	end

	if gSpawnTimer:elapsed_sec() > 2 then 
		local val = math.random(1, 3)
		if val == 1 then 
			local asteroid = Asteroid:Create("asteroid_big")
			AddAsteroid(asteroid)
		elseif val == 2 then 
			local asteroid = Asteroid:Create("asteroid_small")
			AddAsteroid(asteroid)
		elseif val == 3 then 
			-- TODO: Create new ship Enemy
		end
		gSpawnTimer:stop()
	end
end

--[[
	Clears out the Asteroids table and kills any remaing 
	asteroid entities
--]]
function ResetAsteroids()
	for k, v in pairs(Asteroids) do 
		local asteroid = Entity(v.m_EntityID)
		asteroid:kill()
		Asteroids[k] = nil
	end
end
----------------------------------------------------------------------------

----------------------------------------------------------------------------
-- Projectile Holder Helper Functions
----------------------------------------------------------------------------
Projectiles = {}
function AddProjectile(projectile)
	table.insert(Projectiles, projectile)
end

function UpdateProjectiles()
	for k, v in pairs(Projectiles) do
		if v:TimesUp() then 
			v:Destroy()
			Projectiles[k] = nil
		else
			v:Update()
		end
	end
end

--[[
	Clears out the Projectile table and kills any remaing 
	projectile entities
--]]
function ResetProjectiles()
	for k, v in pairs(Projectiles) do 
		local projectile  = Entity(v.m_EntityID)
		projectile:kill()
		Projectiles[k] = nil
	end
end
----------------------------------------------------------------------------

--[[
	Gets the value of the specific digit for 
	the number that is sent in. For instance,
	if the number is 9596 and we want digit three,
	it would return 5.
--]]
function GetDigit(num, digit)
	local n = 10 ^ digit 
	local n1 = 10 ^ (digit - 1)
	return math.floor((num % n) / n1)
end