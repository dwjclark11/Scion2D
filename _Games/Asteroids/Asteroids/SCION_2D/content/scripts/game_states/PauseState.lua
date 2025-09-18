--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

--[[
PauseState
----------
@brief Represents the pause screen state. Displays a centered "PAUSED" 
label on screen, halts music playback, and waits for player input 
to resume the game.

Responsibilities:
- Display a visible "PAUSED" label in the center of the screen
- Pause and resume background music
- Handle user input to exit the pause state and resume gameplay
]]
PauseState = S2D_Class("PauseState")

-----------------------------------------------------------------------------------------------------------------
-- @brief Initialize the PauseState
-- Creates and centers a "PAUSED" label, sets up state callbacks, and stores stack reference
-- @param params Table of parameters, requires `stack` for state management
-----------------------------------------------------------------------------------------------------------------
function PauseState:Init(params)
	params = params or {}

	-- Create the "PAUSED" label entity
	self.pausedLabel = Entity("PausedLabel", "")
	self.pausedLabel:addComponent(UIComp())
	local text = self.pausedLabel:addComponent(TextComponent("Future-32", "PAUSED", Color(255, 255, 0, 255), 0, 0))
	local transform = self.pausedLabel:addComponent(Transform(vec2(0, 0), vec2(2, 2), 0.0))
	
	-- Get the size of the text box for centering 
	local textX, textY = S2D_MeasureText(text, transform)
	
	-- Calculate centered screen position
	local newPos = vec2(
		(S2D_WindowWidth() * 0.5) - (textX * 0.5),
		(S2D_WindowHeight() * 0.5) - (textY * 0.5)
	)
	
	-- Apply centered position
	transform.position = newPos
	
	self.stack = params.stack 
	self.state = State("pause")
	
	self.state:setVariableTable(self)
	self.state:setOnEnter( function() self:OnEnter() end )
	self.state:setOnExit( function() self:OnExit() end )
	self.state:setOnUpdate( function(dt) self:Update(dt) end )
	self.state:setHandleInputs( function() self:HandleInputs() end )
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Get the underlying State object
-- @return The State instance representing this PauseState
-----------------------------------------------------------------------------------------------------------------
function PauseState:GetState()
	return self.state 
end 

-----------------------------------------------------------------------------------------------------------------
-- @brief Called when entering the pause state
-- Pauses background music and plays an "on" sound effect
-----------------------------------------------------------------------------------------------------------------
function PauseState:OnEnter()
	Music.pause()
	Sound.play("on")
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Called when exiting the pause state
-- Destroys the "PAUSED" label, plays an "off" sound, and resumes music if it was paused
-----------------------------------------------------------------------------------------------------------------
function PauseState:OnExit()
	if self.pausedLabel then
		self.pausedLabel:destroy()
	end
	Sound.play("off")
	
	if Music.isPaused() then 
		Music.resume()
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Update logic for the pause state
-- Currently does nothing (pause is static), but left for consistency
-- @param dt Delta time since last frame
-----------------------------------------------------------------------------------------------------------------
function PauseState:Update(dt)
	
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Handle player input while paused
-- Exits the pause state (pops from stack) when Backspace or Gamepad Start is pressed
-----------------------------------------------------------------------------------------------------------------
function PauseState:HandleInputs()
	if Keyboard.justReleased(KEY_BACKSPACE) or (Gamepad.connected(1) and Gamepad.justReleased(1, GP_BTN_START)) then 
		self.stack:pop()
	end
end
