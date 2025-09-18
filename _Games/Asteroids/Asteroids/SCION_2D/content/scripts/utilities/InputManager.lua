--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

--[[
InputManager
------------
Handles player input from both keyboard and gamepad devices. 
It abstracts input so the game logic doesn’t need to check for 
specific devices — instead, it queries common actions like 
move, rotate, fire, and boost.

Responsibilities:
- Track whether any gamepad is connected
- Assign input controls to a player number
- Provide unified input queries (forward, rotate, fire, boost)
- Handle gamepad connection/disconnection events
]]
InputManager = S2D_Class("InputManager")

-- Initialize the input manager
-- params: optional table with playerNum (player index to bind inputs to)
function InputManager:Init(params)
	params = params or {}
	-- Track if any gamepad is currently connected
	self.bAnyGamepadConnected = Gamepad.anyConnected()
	-- Player number determines which keyboard/gamepad mapping to use
	self.playerNum = params.playerNum or 0
	-- Register event handler for gamepad connection state changes
	self.gamepadConnectHandler = GamepadConnectEventHandler( function (event) self:OnConnectionChanged(event) end )
	EventDispatcher(DispatcherType.Scion):addHandler(self.gamepadConnectHandler, GamepadConnectEvent )
end

-- Set which player this input manager belongs to
function InputManager:SetPlayerNum(playerNum)
	self.playerNum = playerNum
end

-- Check if the player is moving forward
function InputManager:MoveForward()
	if self.bAnyGamepadConnected then 
		if not Gamepad.connected(self.playerNum) then 
			S2D_warn("Gamepad [%d] is not connected", self.playerNum)
			return false
		end
	
		return Gamepad.pressed(self.playerNum, DPAD_UP)
	end
	
	if self.playerNum == 1 then 
		return Keyboard.pressed(KEY_W)
	end
	
	return false
end

-- Check if the player is rotating counter-clockwise
function InputManager:RotateCCW()
	if self.bAnyGamepadConnected then 
		if not Gamepad.connected(self.playerNum) then 
			S2D_warn("Gamepad [%d] is not connected", self.playerNum)
			return false
		end
		
		return Gamepad.pressed(self.playerNum, DPAD_LEFT)
	end 
	
	if self.playerNum == 1 then 
		return Keyboard.pressed(KEY_A)
	end
	
	return false
end

-- Check if the player is rotating clockwise
function InputManager:RotateCW()
	if self.bAnyGamepadConnected then 
		if not Gamepad.connected(self.playerNum) then 
			S2D_warn("Gamepad [%d] is not connected", self.playerNum)
			return false
		end
		
		return Gamepad.pressed(self.playerNum, DPAD_RIGHT)
	end
	
	if self.playerNum == 1 then 
		return Keyboard.pressed(KEY_D)
	end
	
	return false
end

-- Check if the player is firing a projectile
function InputManager:Fire()
	if self.bAnyGamepadConnected then 
		if not Gamepad.connected(self.playerNum) then 
			S2D_warn("Gamepad [%d] is not connected", self.playerNum)
			return false
		end
		
		return Gamepad.pressed(self.playerNum, GP_BTN_A)
	end
	
	if self.playerNum == 1 then 
		return Keyboard.pressed(KEY_SPACE)
	end
	
	return false
end

-- Check if the player is boosting
function InputManager:Boost()
	if self.bAnyGamepadConnected then 
		if not Gamepad.connected(self.playerNum) then 
			S2D_warn("Gamepad [%d] is not connected", self.playerNum)
			return false
		end
		
		return Gamepad.justPressed(self.playerNum, GP_RSHOULDER)
	end
	
	if self.playerNum == 1 then 
		return Keyboard.pressed(KEY_RSHIFT)
	end
	
	return false
end

function InputManager:Rumble(left, right, duration)
	if self.bAnyGamepadConnected then 
		if not Gamepad.connected(self.playerNum) then 
			S2D_warn("Gamepad [%d] is not connected", self.playerNum)
			return false
		end
		
		if not Gamepad.isRumbleSupported(self.playerNum) then 
			return false 
		end
		
		return Gamepad.rumble(self.playerNum, left, right, duration)
	end
end

-- Handle gamepad connection or disconnection events
function InputManager:OnConnectionChanged(event)
	if event.type == GamepadConnectType.Connected then 
		self.bAnyGamepadConnected = true 
	elseif event.type == GamepadConnectType.Disconnected then
		self.bAnyGamepadConnected = false
	end		
end
