--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

--[[
GameState
---------
@brief Represents the active gameplay state of the game. 
This state is responsible for setting up the main scene, 
managing gameplay systems (ships, asteroids, projectiles, pickups), 
handling collisions, updating the HUD, and transitioning 
to other states (Pause, Game Over).

Responsibilities:
- Load and initialize the main game scene
- Manage player ships, HUD, and collision system
- Update all gameplay systems every frame
- Handle inputs for pausing or exiting the game
- Transition to Game Over state when conditions are met
]]
GameState = S2D_Class("GameState")

-----------------------------------------------------------------------------------------------------------------
-- @brief Initialize the GameState with references and default values
-- @param params Table containing initialization parameters (e.g., state stack)
-----------------------------------------------------------------------------------------------------------------
function GameState:Init(params)
	local reg = Registry()
	self.stack = params.stack 
	self.state = State("game")
	
	self.playerShip = nil
	self.collisionSystem = nil
	self.hud = nil
	
	self.state:setVariableTable(self)
	self.state:setOnEnter( function() self:OnEnter() end )
	self.state:setOnUpdate( function(dt) self:Update(dt) end )
	self.state:setHandleInputs( function() self:HandleInputs() end )
	self.bMainLoaded = false -- Prevents reloading scene on multiple entries
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Get the underlying State object
-- @return The State instance representing this GameState
-----------------------------------------------------------------------------------------------------------------
function GameState:GetState()
	return self.state 
end 

-----------------------------------------------------------------------------------------------------------------
-- @brief Called when the GameState is first entered
-- Loads the main scene, spawns player ship, initializes HUD and collision system
-----------------------------------------------------------------------------------------------------------------
function GameState:OnEnter()
	if not self.bMainLoaded then 
		SceneManager.changeScene("main")
		local reg = Registry()
		local playerShip = reg:findEntityByTag("PlayerShip")

		-- Register player ship in the global ship handler
		gShipHandler:AddShip(Ship:Create({ id = playerShip:id() }))

		self.collisionSystem = CollisionSystem:Create()
		self.hud = Hud:Create()

		-- Start background music if available
		local sceneMusic = SceneManager.getDefaultMusic()
		if sceneMusic then 
			Music.play(sceneMusic)
		end
		
		self.bMainLoaded = true
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Per-frame update for the GameState
-- Updates all systems and transitions to GameOverState if conditions are met
-- @param dt Delta time (time since last frame)
-----------------------------------------------------------------------------------------------------------------
function GameState:Update(dt)
	if self.bMainLoaded then 
		-- Update gameplay systems
		gShipHandler:Update()
		gAsteroidHandler:Update()
		gProjectileHandler:Update()
		gPickupHandler:Update()
		
		self.collisionSystem:Update()
		self.hud:Update()

		-- Transition to GameOverState if the game has ended
		if gData:IsGameOver() then
			self.stack:push(GameOverState:Create({stack = self.stack, hud = self.hud }):GetState())
			return
		end
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Handle input specific to the GameState
-- Allows pausing the game via Backspace or Gamepad Start button
-----------------------------------------------------------------------------------------------------------------
function GameState:HandleInputs()
	if Keyboard.justReleased(KEY_BACKSPACE) or (Gamepad.connected(1) and Gamepad.justReleased(1, GP_BTN_START)) then 
		self.stack:push(PauseState:Create({stack = self.stack}):GetState())
		return
	end
end

