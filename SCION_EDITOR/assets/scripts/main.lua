-- Main Lua Script!
gEntity = Entity("TestEntity", "Groupy")
gEntity2 = Entity("BigTesty", "Groupy")
local transform = gEntity:add_component(
	Transform(vec2(100, 100), vec2(4, 4), 0)
)

gEntity2:add_component(Transform(200, 100, 4, 4, 0))
local sprite2 = gEntity2:add_component(Sprite("red_player", 32.0, 32.0, 0, 2, 0))
sprite2:generate_uvs()

gEntity2:add_component(Animation(6, 15, 0, false))

local sprite = gEntity:add_component(
	Sprite("castle", 16.0, 16.0, 0, 28, 0)
)
sprite:generate_uvs()

local rotation = 0.0
local x_pos = 10.0
local scale = 1.0
local move_right = true
local value = 0

local view = Registry.get_entities(Transform)

view:exclude(Transform)

view:for_each(
	function (entity)
		print(entity:name())
	end
)

function UpdateEntity()

end

main = {
	[1] = {
		update = function()
			if rotation >= 360 then
				rotation = 0
			end 

			if move_right and x_pos < 300 then 
				x_pos = x_pos + 3 
				value = value + 0.01
			elseif move_right and x_pos >= 300 then 
				move_right = false
			end 

			if not move_right and x_pos > 10 then 
				x_pos = x_pos - 3
				value = value - 0.01 
			elseif not move_right  and x_pos <= 10 then 
				move_right = true 
			end 

			
			transform.position.x = x_pos
			transform.rotation = rotation 
			
			if move_right then 
				rotation = rotation + 9
			else
				rotation = rotation - 9
			end 

			scale = math.sin(value) * 10 

			if scale >= 10 then 
				scale = 10 
			elseif scale <= 1 then 
				scale = 1 
			end 

			if value >= 1 then 
				value = 1 
			elseif value <= 0 then
				value = 0
			end 

			transform.scale = vec2(scale, scale)

			-- [[
			if Keyboard.pressed(KEY_W) then 
				print("Key W was just pressed")
			elseif Keyboard.pressed(KEY_S) then 
				print("Key S was just pressed")
			elseif Keyboard.pressed(KEY_A) then 
				print("Key A was just pressed")
			elseif Keyboard.pressed(KEY_D) then 
				print("Key D was just pressed")
			end

			if Mouse.just_pressed(LEFT_BTN) then
				print("Mouse Left Button Pressed")
			elseif Mouse.just_pressed(MIDDLE_BTN) then
				print("Mouse MIDDLE Button Pressed") 
			elseif Mouse.just_pressed(RIGHT_BTN) then
				print("Mouse RIGHT Button Pressed") 
			end

			local rstick_LR = Gamepad.get_axis_position(1, 2)
			print("RSTICK LR value: " ..rstick_LR)
			local rstick_UD = Gamepad.get_axis_position(1, 3)
			print("RSTICK UD value: " ..rstick_UD)

			local lstick_LR = Gamepad.get_axis_position(1, 0)
			print("LSTICK LR value: " ..lstick_LR)
			local lstick_UD = Gamepad.get_axis_position(1, 1)
			print("LSTICK UD value: " ..lstick_UD)

			local lTrigger = Gamepad.get_axis_position(1, 4)
			print("LTrigger value: " ..lTrigger)
			local rTrigger = Gamepad.get_axis_position(1, 5)
			print("RTrigger value: " ..rTrigger)
			--]]
		end
	},
	[2] = {
		render = function()
			
		end
	},
}