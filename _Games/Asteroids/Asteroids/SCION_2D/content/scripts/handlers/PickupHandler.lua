--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-- Tracks all active pickups, keyed by entity id
PickupHandler = S2D_Class("PickupHandler")

function PickupHandler:Init()
	self.pickups = {}
	self.count = 0
end

function PickupHandler:NumPickups()
	return self.count
end

function PickupHandler:AddPickup(pickup)
	self.pickups[pickup:GetID()] = pickup
	self.count = self.count + 1
end

function PickupHandler:Update()
	for _, pickup in pairs(self.pickups) do
		pickup:Update()
	end
end

-- Applies the pickup to the ship and removes it. Does nothing if it is already gone
function PickupHandler:OnHandlePickup(pickup_id, ship)
	local pickup = self.pickups[pickup_id]
	if pickup then
		pickup:OnOverlap(ship)
		self.pickups[pickup_id] = nil
		self.count = self.count - 1
	end
end

function PickupHandler:ResetPickups()
	for id, pickup in pairs(self.pickups) do
		Entity(pickup:GetID()):destroy()
		self.pickups[id] = nil
	end

	self.count = 0
end

gPickupHandler = PickupHandler:Create()
