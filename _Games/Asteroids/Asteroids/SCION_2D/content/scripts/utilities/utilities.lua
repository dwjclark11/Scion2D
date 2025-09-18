--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-------------------------------------------------------------------
-- @brief Simple Constants
-- Defines core window dimensions and scoring values.
-------------------------------------------------------------------
WINDOW_WIDTH = S2D_WindowWidth()
WINDOW_HEIGHT = S2D_WindowHeight()
SMALL_ASTEROID_SCORE = 15
LARGE_ASTEROID_SCORE = 100
-------------------------------------------------------------------

-------------------------------------------------------------------
-- @brief Loads an entity definition into a new Entity.
-- @param def Table containing entity tag, group, and component data.
-- @return integer ID of the newly created entity.
-------------------------------------------------------------------
function LoadEntity(def)
	assert(def, "Def does not exist")

	local tag = def.tag or ""
	local group = def.group or ""

	local newEntity = Entity(tag, group)

	if def.components.transform then
		newEntity:addComponent(
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
		local sprite = newEntity:addComponent(
			Sprite(
				def.components.sprite.asset_name,
				def.components.sprite.width,
				def.components.sprite.height,
				def.components.sprite.start_x,
				def.components.sprite.start_y,
				def.components.sprite.layer
			)
		)
		sprite:generateUVs()
		sprite.bHidden = def.components.sprite.bHidden or false
	end

	if def.components.circle_collider then
		newEntity:addComponent(
			CircleCollider(
				def.components.circle_collider.radius
			)
		)
	end

	if def.components.animation then 
		newEntity:addComponent(
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

-------------------------------------------------------------------
-- @brief Wraps an entity’s position around screen bounds.
-- @param position vec2 Current position of entity.
-- @param width number Width of entity.
-- @param height number Height of entity.
-------------------------------------------------------------------
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

-------------------------------------------------------------------
-- @brief Generates a random 2D velocity vector.
-- @param min_speed number Minimum speed value.
-- @param max_speed number Maximum speed value.
-- @return vec2 Random velocity vector.
-------------------------------------------------------------------
function GetRandomVelocity(min_speed, max_speed)
	return vec2(
		math.random(min_speed, max_speed),
		math.random(min_speed, max_speed)
	)
end

-------------------------------------------------------------------
-- @brief Generates a random position within screen bounds.
-- @return vec2 Random screen position.
-------------------------------------------------------------------
function GetRandomPosition()
	return vec2(
		math.random(WINDOW_WIDTH) + WINDOW_WIDTH,
		math.random(WINDOW_HEIGHT) + WINDOW_HEIGHT
	)
end
----------------------------------------------------------------------------

-------------------------------------------------------------------
-- @brief Gets the value of a digit in a number.
-- @param num number Input number.
-- @param digit integer Position of the digit (1 = rightmost).
-- @return integer Value of the digit at the given position.
-- Example: GetDigit(9596, 3) → 5
-------------------------------------------------------------------
function GetDigit(num, digit)
	local n = 10 ^ digit 
	local n1 = 10 ^ (digit - 1)
	return math.floor((num % n) / n1)
end

-------------------------------------------------------------------
-- @brief Toggles sprite visibility on/off like a flash.
-- @param sprite Sprite The sprite component to flash.
-- @param timer Timer A timer instance used for toggling.
-- @param time number Interval in milliseconds (must be > 0).
-------------------------------------------------------------------
function FlashSprite(sprite, timer, time)
	if time <= 0 then 
		S2D_warn("time value in FlashSprite, cannot be zero or less")
		return
	end
    -- Start the timer if it isn't already running
    if not timer:is_running() then
        timer:start()
    end

    -- Get elapsed time in ms
    local elapsed = timer:elapsed_ms()

    -- Determine flashing state (1 second on, 1 second off)
    if (math.floor(elapsed / time) % 2) == 0 then
        sprite.bHidden = false  -- Visible
    else
        sprite.bHidden = true   -- Hidden
    end
end

-------------------------------------------------------------------
-- @brief Flashes a sprite’s color between two values.
-- @param sprite Sprite The sprite component to color flash.
-- @param timer Timer A timer instance used for toggling.
-- @param time number Interval in milliseconds (must be > 0).
-- @param color1 Color First color value.
-- @param color2 Color Second color value.
-------------------------------------------------------------------
function FlashColor(sprite, timer, time, color1, color2)
	if time <= 0 then 
		S2D_warn("time value in FlashColor, cannot be zero or less")
		return
	end
    -- Start the timer if it isn't already running
    if not timer:is_running() then
        timer:start()
    end

    -- Get elapsed time in ms
    local elapsed = timer:elapsed_ms()

    -- Determine flashing state (1 second on, 1 second off)
    if (math.floor(elapsed / time) % 2) == 0 then
        sprite.color = color1  
    else
        sprite.color = color2
    end
end

