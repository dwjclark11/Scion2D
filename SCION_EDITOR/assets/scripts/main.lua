-- Main Lua Script!
run_script("assets/scripts/TestProject/assetDefs.lua")
run_script("assets/scripts/TestProject/testmap.lua")
run_script("assets/scripts/TestProject/test_platformer.lua")
run_script("assets/scripts/utilities.lua")
run_script("assets/scripts/rain_generator.lua")
run_script("assets/scripts/follow_cam.lua")
run_script("assets/scripts/events/event_manager.lua")
run_script("assets/scripts/events/collision_event.lua")
run_script("assets/scripts/systems/trigger_system.lua")

-- [[
local tilemap = CreateTestPlatformerMap()
assert(tilemap)
LoadAssets(AssetDefs)
LoadMap(tilemap)

gCollisionEvent = CollisionEvent:Create() 
gTriggerSystem = TriggerSystem:Create() 
gCollisionEvent:SubscribeToEvent(gTriggerSystem)


local rainGen = RainGenerator:Create(
	{
		scale = 0.5,
		rain_vel_min = 10,
		rain_vel_max = 100,
		rain_life_min = 250,
		rain_life_max = 500
	}
)
Sound.play("rain", -1, 1)
Sound.set_volume(1, 30)

Music.play("snowfall", -1)
Music.set_volume(50)

-- Create dimmer and lightning 
local darkness = Entity("", "")
darkness:add_component(Transform(vec2(0, 0), vec2(WindowWidth() / 16, WindowHeight() / 16), 0))

local darkSprite = darkness:add_component(Sprite("white_box", 16, 16, 0, 0, 4))
darkSprite.color = Color(0, 0, 0, 100)
darkSprite:generate_uvs()

gTimer = Timer()
gTimer:start()

function UpdateDimmer(entity)
	local darkSprite = entity:get_component(Sprite)

	if gTimer:elapsed_ms() > 3000 then 
		darkSprite.color = Color(255, 255, 255, 25)
	end
	
	if gTimer:elapsed_ms() > 3100 and gTimer:elapsed_ms() < 3200 then 
		darkSprite.color = Color(0, 0, 0, 100)
	elseif gTimer:elapsed_ms() > 3200 and gTimer:elapsed_ms() < 3400 then 
		darkSprite.color = Color(255, 255, 255, 25)
	elseif gTimer:elapsed_ms() > 3400 then 
		darkSprite.color = Color(0, 0, 0, 100)
		gTimer:stop()
		gTimer:start()
	end
end

-- Create Simple player 
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

bLeft = false

function UpdatePlayer(player)
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

	sprite.uvs.v = sprite.start_y * sprite.uvs.uv_height
	
end

-- Create a follow camera for the player
Camera.get().set_scale(2.0)

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
--]]

--[[
-- Test Data
local objectData = ObjectData("test_tag", "test_group", true, true, 9919)
local userData = UserData.create_user_data(objectData);

local objData1 = userData:get_user_data()
print(objData1:to_string() .."\n")

userData:set_user_data(ObjectData("New Tag", "Newer group", false, true, 12112))

local objData2 = userData:get_user_data()
print(objData2:to_string())
--]]


main = {
	[1] = {
		update = function()
			-- [[
			UpdatePlayer(gPlayer)
			gFollowCam:update()
			rainGen:Update(0.016) -- Add delta-time
			UpdateDimmer(darkness)
		

			Debug()
			--]]
			
			local uda, udb = ContactListener.get_user_data()
			if uda and udb then 
			-- [[
			print("uda: Tag: " ..uda.tag ..", Group: " ..uda.group)
			for k, v in pairs(uda.contactEntities) do 
				
				print("uda contacts: Tag: " ..v.tag ..", Group: " ..v.group)
			end

			print("udb: Tag: " ..udb.tag ..", Group: " ..udb.group)
			for k, v in pairs(udb.contactEntities) do 
				print("udb contacts: Tag: " ..v.tag ..", Group: " ..v.group)
			end
			--]]
				--gCollisionEvent:EmitEvent(uda, udb)
			end 

		end
	},
	[2] = {
		render = function()

		end
	},
}