--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

--[[
PlayerShipHandler
-----------------
@brief Manages all player ships in the game. 
Responsible for registering, updating, retrieving, and handling events 
related to player ships. Supports a maximum of two ships (e.g., Player 1 
and Player 2).

Responsibilities:
- Track and manage up to 2 active player ships
- Update ship states each frame
- Relay and handle ship-related events
- Provide ship lookup by ID or index
- Manage damage, destruction, and reset behaviors
]]
PlayerShipHandler = S2D_Class("PlayerShipHandler")

-----------------------------------------------------------------------------------------------------------------
-- @brief Initialize the player ship handler with no ships registered
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:Init()
	self.playerCount = 0	-- Tracks how many player ships are currently registered
	self.ships = {}			-- Holds references to all player ships
	
	self.pickupDispatcher = EventDispatcher(DispatcherType.Lua)
	self.shipEventHandler = LuaEventHandler( function (event) self:HandleShipEvents(event) end )
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Get the number of registered player ships
-- @return Integer count of current player ships
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:GetCount()
	return self.playerCount
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Add a custom event handler for ship-related events
-- @param eventHandler The handler function or object
-- @param eventType (optional) The type of event to handle (defaults to LuaEvent)
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:AddEventHandler(eventHandler, eventType)
	eventType = eventType or LuaEvent
	self.pickupDispatcher:addHandler(eventHandler, eventType)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Add a new ship to the handler
-- Prevents adding more than 2 ships (Player 1 and Player 2)
-- @param ship The ship object to add
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:AddShip(ship)
	if self.playerCount >= 2 then 
		S2D_warn("Trying to add another ship. Only 2 ships allowed")
		return 
	end 
	
	table.insert(self.ships, ship)				-- Add the ship to the list
	self.playerCount = self.playerCount + 1		-- Increment count of ships
	ship:SetPlayerNum(self.playerCount)
	ship:AddEventHandler(self.shipEventHandler, LuaEvent)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Update all registered player ships
-- Should be called each frame to process ship logic
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:Update()
	for _, v in pairs(self.ships) do
		v:Update()
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Retrieve a ship by its unique entity ID
-- @param id The entity ID of the ship
-- @return The ship object if found, otherwise nil
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:GetShipByID(id)
	for k, v in pairs(self.ships) do 
		if v:GetID() == id then 
			return self.ships[k]
		end 
	end 
	return nil
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Retrieve a ship at a given index (1-based)
-- @param index The index of the ship in the ships table
-- @return The ship object at the given index
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:GetShipAtIndex(index)
	return self.ships[index]
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Check if there are any ships available with remaining lives
-- @return True if at least one ship has lives left, false otherwise
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:ShipsAvailable()
	for k, v in pairs(self.ships) do 
		if v:GetLives() > 0 then 
			return true
		end 
	end
	
	return false
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Apply damage to a ship based on collision data
-- @param colData Table containing { id, damage }
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:TakeDamage(colData)
	assert(colData, "Collision Data was not sent in correctly or is invalid.")
	
	local ship = self:GetShipByID(colData.id)
	if ship then 
		if ship:TakeDamage(colData.damage) then
			self:DestroyShip(colData.id)
		end
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Destroy a ship by ID and emit a life event
-- @param id The entity ID of the ship to destroy
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:DestroyShip(id)
	local ship = self:GetShipByID(id)
	if ship then 
		ship:OnShipDestroy()
		self.pickupDispatcher:emitEvent( 
			LuaEvent( 
				{ 	
					type = ShipEventType.LifeEvent, 
					event = ShipLifeEvent:Create({ id = id } )
				}
			)
		)
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Reset all ships to their initial state
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:ResetAllShips()
	for k, v in pairs(self.ships) do 
		 v:Reset()
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Handle and relay incoming ship events
-- @param event The event object to dispatch
-----------------------------------------------------------------------------------------------------------------
function PlayerShipHandler:HandleShipEvents(event)
	-- Relay event to other handlers
	self.pickupDispatcher:emitEvent( event )
end

gShipHandler = PlayerShipHandler:Create()
--------------------------------------------------------------------------------------
