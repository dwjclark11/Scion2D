--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

--- @class GameOverState
--- @brief Handles the "Game Over" screen state, including UI setup,
--- asteroid animation, resetting game systems, and handling input
--- to return to gameplay or menus.
GameOverState = S2D_Class("GameOverState")

--- @brief Initializes the Game Over state
--- @param params table Optional parameters, must include {stack = gStack, hud = hudInstance}
function GameOverState:Init(params)
	params = params or {}
	-- The state stack must be sent in during state:Create({stack = gStack})
	self.stack = params.stack
	self.state = State("game_over")
	self.state:setVariableTable(self)
	self.state:setOnEnter( function() self:OnEnter() end )
	self.state:setOnExit( function() self:OnExit() end )
	self.state:setOnUpdate( function(dt) self:OnUpdate(dt) end )
	self.state:setOnRender( function() self:OnRender() end )
	self.state:setHandleInputs( function() self:HandleInputs() end )
	
	-- Setup "Game Over" UI label
	self.gameOverLabel = Entity("game_over", "")
	self.gameOverLabel:addComponent(UIComp())
	self.gameOverLabel:addComponent(
		Transform(
			vec2(
				(S2D_WindowWidth() * 0.5) - 256, 
				(S2D_WindowHeight() * 0.5) - 32
			), 
			vec2(4, 4), 0.0
		)
	)
	
	local gameOverSprite = self.gameOverLabel:addComponent(Sprite("game_over", 128, 16, 0, 0, 2))
	gameOverSprite:generateUVs()
	gameOverSprite.bHidden = true
	self.hud = params.hud 
	
	-- Setup asteroid animation entity
	self.asteroid = Entity("game_over_asteroid", "")
	self.asteroid:addComponent(UIComp())
	local asteroidTransform = self.asteroid:addComponent(Transform(vec2(0, 0), vec2(1, 1), 0.0))
	
	local asteroidSprite = self.asteroid:addComponent(Sprite("asteroid_small", 29, 26, 0, 0, 1))
	asteroidSprite:generateUVs() 
	
	asteroidTransform.position = vec2(
		(S2D_WindowWidth() * 0.5) - (asteroidSprite.width * 0.5),
		(S2D_WindowHeight() * 0.5) - (asteroidSprite.height * 0.5)
	)
	
	self.bBrokenScreen = false
	
end

--- @brief Gets the internal State object for this Game Over state
--- @return State The wrapped State instance
function GameOverState:GetState()
	return self.state
end

--- @brief Called when entering the Game Over state
function GameOverState:OnEnter()
	Music.play("game_over")
	
	-- Disable asteroid spawning
	gAsteroidHandler:Disable()
end

--- @brief Called when exiting the Game Over state
function GameOverState:OnExit()
	self.gameOverLabel:destroy()
	self.asteroid:destroy()
	
	-- Reset all data back to nothing and refresh the game
	gData:Reset()
	gAsteroidHandler:ResetAsteroids()
	gProjectileHandler:Reset()
	gPickupHandler:ResetPickups()
	self.hud:Reset()
	gShipHandler:ResetAllShips()
	
	Music.play("space")
end

--- @brief Updates the Game Over state each frame
--- @param dt number Delta time in seconds
function GameOverState:OnUpdate(dt)
	
	gShipHandler:Update()
	gAsteroidHandler:Update()
	gProjectileHandler:Update()
	
	self:UpdateScreenAnim()
end 

--- @brief Renders the Game Over state (currently empty)
function GameOverState:OnRender()
end

--- @brief Handles input for the Game Over state
--- Pressing Enter or Start will exit this state
function GameOverState:HandleInputs()
	if Keyboard.justReleased(KEY_ENTER) or (Gamepad.connected(1) and Gamepad.justReleased(1, GP_BTN_START)) then 
		self.stack:pop()
		return 
	end
end

--- @brief Updates the asteroid screen animation and triggers "Game Over" reveal
function GameOverState:UpdateScreenAnim()
	
	if self.bBrokenScreen then
		return 
	end 
	
	local asteroidSprite = self.asteroid:getComponent(Sprite)
	local asteroidTransform = self.asteroid:getComponent(Transform)
	if asteroidTransform.scale.x < 15.0 then 
		-- Grow and spin the asteroid
		asteroidTransform.scale = asteroidTransform.scale + 0.2
		asteroidTransform.position = vec2(
			(S2D_WindowWidth() * 0.5) - (asteroidSprite.width * 0.5 * asteroidTransform.scale.x),
			(S2D_WindowHeight() * 0.5) - (asteroidSprite.height * 0.5 * asteroidTransform.scale.y)
		)
		
		asteroidTransform.rotation = asteroidTransform.rotation + 15
		
	elseif not self.bBrokenScreen then
		-- Break screen effect and reveal "Game Over" text
		self.bBrokenScreen = true 
		self.hud:ShowDamage(true)
		Sound.play("crack_screen")
		local gameOverSprite = self.gameOverLabel:getComponent(Sprite)
		gameOverSprite.bHidden = false
	end
end
