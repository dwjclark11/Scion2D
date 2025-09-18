--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-----------------------------------------------------------------------------------------------------------------
-- @class TitleState
-- @brief Handles the title screen state, including centering the title label,
-- playing music, flashing the title text, and transitioning to gameplay.
-----------------------------------------------------------------------------------------------------------------
TitleState = S2D_Class("TitleState")

-----------------------------------------------------------------------------------------------------------------
-- @brief Initializes the Title state
-- @param params table Must include {stack = stateStack}
-----------------------------------------------------------------------------------------------------------------
function TitleState:Init(params)
	
	local reg = Registry()
	self.stack = params.stack 
	self.state = State("title")
	self.titleLabel = reg:findEntityByTag("TitleLabel")
	
	-- Verify the title label is centered
	local transform = self.titleLabel:getComponent(Transform)
	local text = self.titleLabel:getComponent(TextComponent)
	
	local textX, textY = S2D_MeasureText(text, transform)
	
	-- Calculate the new position to center the text to the screen
	local newPos = vec2(
		(S2D_WindowWidth() * 0.5) - (textX * 0.5),
		transform.position.y
	)
	transform.position = newPos
	
	self.state:setVariableTable(self)
	self.state:setOnEnter( function() self:OnEnter() end )
	self.state:setOnExit( function() self:OnExit() end )
	self.state:setOnUpdate( function(dt) self:Update(dt) end )
	self.state:setHandleInputs( function() self:HandleInputs() end )
	self.flashTimer = Timer()
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Gets the internal State object
-- @return State Wrapped State instance
-----------------------------------------------------------------------------------------------------------------
function TitleState:GetState()
	return self.state 
end 

-----------------------------------------------------------------------------------------------------------------
-- @brief Called when entering the Title state
-- Plays background music if availabe
-----------------------------------------------------------------------------------------------------------------
function TitleState:OnEnter()
	-- Start background music if available
	local sceneMusic = SceneManager.getDefaultMusic()
	if sceneMusic then 
		Music.play(sceneMusic)
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Called when exiting the Title state
-----------------------------------------------------------------------------------------------------------------
function TitleState:OnExit() 
	
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates the Title state
-- @param dt number Delta time in seconds
-----------------------------------------------------------------------------------------------------------------
function TitleState:Update(dt)
	local text = self.titleLabel:getComponent(TextComponent)
	FlashSprite(text, self.flashTimer, 500)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Handles input for the Title state
-- Allows starting the game VIA Enter or Gamepad Start Button
-----------------------------------------------------------------------------------------------------------------
function TitleState:HandleInputs()
	if Keyboard.justReleased(KEY_ENTER) or (Gamepad.connected(1) and Gamepad.justReleased(1, GP_BTN_START)) then 
		self.stack:changeState(GameState:Create({ stack = self.stack}):GetState())
		return
	end
end

