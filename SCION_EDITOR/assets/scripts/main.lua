-- Main Lua Script!

gEntity = Entity("TestEntity", "Groupy")
gEntity2 = Entity("BigTesty", "Groupy")
local transform = gEntity:add_component(
	Transform(vec2(100, 100), vec2(10, 10), 0)
)

gEntity2:add_component(Transform(200, 100, 10, 10, 0))

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

			--local sprite2 = gEntity:get_component(Sprite)
			--print("Texture Name: " ..sprite2.texture_name)

		end
	},
	[2] = {
		render = function()
			
		end
	},
}