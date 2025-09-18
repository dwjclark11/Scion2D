--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-----------------------------------------------------------------------------------------------------------------
--- @class PickupHandler
--- @brief Manages active pickup objects: spawn, update, collection, and cleanup.
--- @field pickups table<number, any> List of pickup instances managed by this handler.
--- @field Create fun(...):PickupHandler Factory method provided by S2D_Class (returns a PickupHandler instance).
-----------------------------------------------------------------------------------------------------------------
PickupHandler = S2D_Class("PickupHandler")

-----------------------------------------------------------------------------------------------------------------
--- @brief Initializes the pickup handler and prepares storage for pickups.
-----------------------------------------------------------------------------------------------------------------
function PickupHandler:Init()
	self.pickups = {}
end


-----------------------------------------------------------------------------------------------------------------
--- @brief Returns the number of pickups currently managed.
--- @return integer count Number of pickups
-----------------------------------------------------------------------------------------------------------------
function PickupHandler:NumPickups()
	return #self.pickups
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Adds a pickup to the managed list.
--- @param pickup any The pickup object to add
-----------------------------------------------------------------------------------------------------------------
function PickupHandler:AddPickup(pickup)
	table.insert(self.pickups, pickup)
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Updates all managed pickups (calls each pickup's Update).
-----------------------------------------------------------------------------------------------------------------
function PickupHandler:Update()
	for k, v in pairs(self.pickups) do 
		v:Update()
	end
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Handles a pickup collection event for a specific ship.
--- @param pickup_id integer The ID of the pickup that was collected
--- @param ship any The ship entity overlapping the pickup
-----------------------------------------------------------------------------------------------------------------
function PickupHandler:OnHandlePickup(pickup_id, ship)
	for k, v in pairs(self.pickups) do 
		if v:GetID() == pickup_id then 
			v:OnOverlap(ship)
			self.pickups[k] = nil
		end
	end
end

-----------------------------------------------------------------------------------------------------------------
--- @brief Clears all pickups and destroys their entities.
-----------------------------------------------------------------------------------------------------------------
function PickupHandler:ResetPickups()
	for k, v in pairs(self.pickups) do 
		local pickup = Entity(v:GetID())
		pickup:destroy()
		self.pickups[k] = nil
	end
end

gPickupHandler = PickupHandler:Create()
