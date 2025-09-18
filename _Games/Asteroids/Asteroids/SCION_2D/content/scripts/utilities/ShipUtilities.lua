------------------------------------------------------
-- @brief Enumeration of different ship event types.
-- Used to classify what kind of event has occurred
-- for a ship (health, life, shield, etc.).
------------------------------------------------------
ShipEventType = S2D_MakeReadOnlyTable(
	"ShipEventType", 
	{
		HealthEvent = 1,
		LifeEvent = 2,
		ShieldEvent = 3,
		-- TODO: add more Ship event types
	}
)

------------------------------------------------------
-- @brief Event class representing a health change.
-- Stores how much health to add or remove, and
-- which ship the event is targeting.
------------------------------------------------------
ShipHealthEvent = S2D_Class("ShipHealthEvent")

------------------------------------------------------
-- @brief Initializes a ShipHealthEvent.
-- @param params Table with fields:
--        - amount: health amount to add/remove (default = 10)
--        - id: ID of the target ship (default = -1)
------------------------------------------------------
function ShipHealthEvent:Init(params)
	params = params or {}
	self.healthAmount = params.amount or 10 -- Defaults to adding health 
	self.shipID = params.id or -1
end

------------------------------------------------------
-- @brief Event class representing a life change.
-- Stores which ship the event is targeting.
------------------------------------------------------
ShipLifeEvent = S2D_Class("ShipLifeEvent")

------------------------------------------------------
-- @brief Initializes a ShipLifeEvent.
-- @param params Table with fields:
--        - id: ID of the target ship
------------------------------------------------------
function ShipLifeEvent:Init(params)
	self.shipID = params.id
end

------------------------------------------------------
-- @brief Event class representing a shield change.
-- Stores how much shield to add or remove, and
-- which ship the event is targeting.
------------------------------------------------------
ShipShieldEvent = S2D_Class("ShipShieldEvent")

------------------------------------------------------
-- @brief Initializes a ShipShieldEvent.
-- @param params Table with fields:
--        - amount: shield amount to add/remove (default = 10)
--        - id: ID of the target ship (default = -1)
------------------------------------------------------
function ShipShieldEvent:Init(params)
	params = params or {}
	self.shieldAmount = params.amount or 10 
	self.shipID = params.id or -1
end
