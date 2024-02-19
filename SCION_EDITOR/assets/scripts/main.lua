-- Main Lua Script!
--run_script("assets/scripts/TestProject/assetDefs.lua")
--run_script("assets/scripts/TestProject/testmap.lua")
--run_script("assets/scripts/utilities.lua")
--
--local tilemap = CreateTestMap()
--assert(tilemap)
--LoadAssets(AssetDefs)
--LoadMap(tilemap)

-- Create the main ball
local ball = Entity("", "")
local circle = ball:add_component(CircleCollider(64.0))
local transform = ball:add_component(Transform( vec2(320, 64), vec2(1, 1), 0))
local physAttr = PhysicsAttributes()

physAttr.eType = BodyType.Dynamic
physAttr.density = 100.0
physAttr.friction = 0.2 
physAttr.restitution = 0.1 
physAttr.radius = circle.radius * (1.0 / 12.0)
physAttr.gravityScale = 2.0 
physAttr.position = transform.position
physAttr.scale = transform.scale 
physAttr.bCircle = true 
physAttr.bFixedRotation = false 

ball:add_component(PhysicsComp(physAttr))

local sprite = ball:add_component(Sprite("soccer_ball", 128, 128, 0, 0, 0))
sprite:generate_uvs()

-----------------------------------------------------------------------------------------
-- Create a box to contain 
-----------------------------------------------------------------------------------------
local bottomEnt = Entity("", "")
local bottomBox = bottomEnt:add_component(BoxCollider(624, 16, vec2(0, 0)))
local bottomTrans = bottomEnt:add_component(Transform(vec2(0, 464), vec2(1, 1), 0))
local bottomPhys = PhysicsAttributes()

bottomPhys.eType = BodyType.Static
bottomPhys.density = 1000.0
bottomPhys.friction = 0.5 
bottomPhys.restitution = 0.0 
bottomPhys.gravityScale = 1.0 
bottomPhys.position = bottomTrans.position
bottomPhys.scale = bottomTrans.scale 
bottomPhys.boxSize = vec2(bottomBox.width, bottomBox.height)
bottomPhys.bBoxShape = true 
bottomPhys.bFixedRotation = true

bottomEnt:add_component(PhysicsComp(bottomPhys))

local leftEnt = Entity("", "")
local leftBox = leftEnt:add_component(BoxCollider(16, 464, vec2(0, 0)))
local leftTrans = leftEnt:add_component(Transform(vec2(0, 0), vec2(1, 1), 0))
local leftPhys = PhysicsAttributes()

leftPhys.eType = BodyType.Static
leftPhys.density = 1000.0
leftPhys.friction = 0.5 
leftPhys.restitution = 0.0 
leftPhys.gravityScale = 1.0 
leftPhys.position = leftTrans.position
leftPhys.scale = leftTrans.scale 
leftPhys.boxSize = vec2(leftBox.width, leftBox.height)
leftPhys.bBoxShape = true 
leftPhys.bFixedRotation = true

leftEnt:add_component(PhysicsComp(leftPhys))

local rightEnt = Entity("", "")
local rightBox = rightEnt:add_component(BoxCollider(16, 480, vec2(0, 0)))
local rightTrans = rightEnt:add_component(Transform(vec2(624, 0), vec2(1, 1), 0))
local rightPhys = PhysicsAttributes()

rightPhys.eType = BodyType.Static
rightPhys.density = 1000.0
rightPhys.friction = 0.5 
rightPhys.restitution = 0.0 
rightPhys.gravityScale = 1.0 
rightPhys.position = rightTrans.position
rightPhys.scale = rightTrans.scale 
rightPhys.boxSize = vec2(rightBox.width, rightBox.height)
rightPhys.bBoxShape = true 
rightPhys.bFixedRotation = true

rightEnt:add_component(PhysicsComp(rightPhys))

local topEnt = Entity("", "")
local topBox = topEnt:add_component(BoxCollider(608, 16, vec2(0, 0)))
local topTrans = topEnt:add_component(Transform(vec2(16, 0), vec2(1, 1), 0))
local topPhys = PhysicsAttributes()

topPhys.eType = BodyType.Static
topPhys.density = 1000.0
topPhys.friction = 0.5 
topPhys.restitution = 0.0 
topPhys.gravityScale = 1.0 
topPhys.position = topTrans.position
topPhys.scale = topTrans.scale 
topPhys.boxSize = vec2(topBox.width, topBox.height)
topPhys.bBoxShape = true 
topPhys.bFixedRotation = true

topEnt:add_component(PhysicsComp(topPhys))
-----------------------------------------------------------------------------------------

function createBall()
	if (Mouse.just_released(LEFT_BTN)) then 
		local pos_x, pos_y = Mouse.screen_position() 
		local ball = Entity("", "")
		local circle = ball:add_component(CircleCollider(64.0))
		local transform = ball:add_component(Transform( vec2(pos_x, pos_y), vec2(0.5, 0.5), 0))
		local physAttr = PhysicsAttributes()

		physAttr.eType = BodyType.Dynamic
		physAttr.density = 100.0
		physAttr.friction = 0.2 
		physAttr.restitution = 0.1 
		physAttr.radius = circle.radius * (1.0 / 12.0)
		physAttr.gravityScale = 2.0 
		physAttr.position = transform.position
		physAttr.scale = transform.scale 
		physAttr.bCircle = true 
		physAttr.bFixedRotation = false 

		ball:add_component(PhysicsComp(physAttr))

		local sprite = ball:add_component(Sprite("soccer_ball", 128, 128, 0, 0, 0))
		sprite:generate_uvs()
	end
end

function updateEntity(entity)
	local physics = entity:get_component(PhysicsComp)
	local transform = entity:get_component(Transform)

	local velocity = physics:get_linear_velocity()

	if velocity.y > 0.0 then 
		physics:set_gravity_scale(15.0)
	else 
		physics:set_gravity_scale(5.0)
	end 

	if Keyboard.pressed(KEY_D) then 
		physics:set_linear_velocity(vec2(25, velocity.y))
	elseif Keyboard.pressed(KEY_A) then 
		physics:set_linear_velocity(vec2(-25, velocity.y))
	end 

	if Keyboard.just_pressed(KEY_W) then 
		physics:set_linear_velocity(vec2(velocity.x, 0))
		physics:linear_impulse(vec2(0, -300000))
	end
end

main = {
	[1] = {
		update = function()
			createBall()
			updateEntity(ball)
		end
	},
	[2] = {
		render = function()

		end
	},
}