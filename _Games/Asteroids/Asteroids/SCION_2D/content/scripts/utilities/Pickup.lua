--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

------------------------------------------------------
-- @brief Enumeration of different pickup types.
-- Provides constants for classifying pickup behavior.
------------------------------------------------------
PickupTypes = S2D_MakeReadOnlyTable("PickupTypes", 
	{
		None = 0,				-- No pickup
		Health = 1,				-- Restores health
		Shield = 2,				-- Restores shield
		WeaponUpgrade = 3,		-- Grants weapon upgrade
		AddLife = 4,			-- Grants an extra life
		-- TODO: add more pickup types
	}
)

------------------------------------------------------
-- @brief Pickup class representing collectible items.
-- Handles spawning, movement, collision, and effects
-- when collected by the player.
------------------------------------------------------
Pickup = S2D_Class("Pickup")

------------------------------------------------------
-- @brief Initializes a pickup entity with given params.
-- @param params Table of parameters (type, soundfx, 
--        position, texture, dimensions, etc.)
------------------------------------------------------
function Pickup:Init(params)
	
	params = params or {}
	self.type = params.type or PickupTypes.Health
	self.velocity = GetRandomVelocity(0, 1)
	self.soundfx = params.soundfx or "collect_1"
	self.pickup = Entity("", params.group or "pickups")
	self.pickup:addComponent(Transform(params.position or vec2(100, 100), vec2(1, 1), 0.0))
	
	local sprite = self.pickup:addComponent(
		Sprite(
			params.sTexture or "default_pickup", 
			params.width or 34, 
			params.height or 33, 
			params.startX or 0,
			params.startY or 0,
			params.layer or 2
		)
	)
	sprite:generateUVs()
	self.pickup:addComponent(CircleCollider(params.radius or 8, vec2(0, 0)))
end

------------------------------------------------------
-- @brief Sets the pickup's position.
-- @param position New position as a vec2.
------------------------------------------------------
function Pickup:SetPosition(position)
	local transform = self.pickup:getComponent(Transform)
	transform.position = position
end

------------------------------------------------------
-- @brief Called when pickup overlaps with a ship.
-- Executes effects depending on pickup type, plays
-- sound, and destroys the pickup entity.
-- @param ship The ship entity that collected the pickup.
------------------------------------------------------
function Pickup:OnOverlap(ship)
	S2D_switch(self.type, 
		{
			[1] = function() ship:AddHealth() end,
			[2] = function() ship:AddShieldHealth() end,
			[3] = function() ship:UpgradeWeapon() end,
			[4] = function() ship:AddLives(1) end,
		},
		function() S2D_warn("Not a valid case!") end
	)
	
	if self.soundfx then 
		Sound.play(self.soundfx)
	end
	
	self.pickup:destroy()
end

------------------------------------------------------
-- @brief Updates pickup each frame.
-- Handles movement, rotation, and screen wrapping.
------------------------------------------------------
function Pickup:Update()
	-- 
	local transform = self.pickup:getComponent(Transform)
	transform.position = transform.position + self.velocity
	transform.rotation = transform.rotation + 1
	
	-- Check if pickup needs to transport to other side of screen
	local sprite = self.pickup:getComponent(Sprite)
	CheckPos(transform.position, sprite.width, sprite.height)
end

------------------------------------------------------
-- @brief Gets the unique ID of the pickup entity.
-- @return ID of the pickup.
------------------------------------------------------
function Pickup:GetID()
	return self.pickup:id()
end

------------------------------------------------------
-- @brief Definitions for different pickup presets.
-- Used for spawning pickups with predefined settings.
------------------------------------------------------
PickupDefs = S2D_MakeReadOnlyTable("PickupDefs",
{
	[1] = {
		type = PickupTypes.Health,
		soundfx = "health_pickup",
		sTexture = "powerup_star_blue",
	},
	[2] = {
		type = PickupTypes.Shield,
		soundfx = "shield_increase",
		sTexture = "powerup_shield_green",
	},
	[3] = {
		type = PickupTypes.WeaponUpgrade,
		soundfx = "weapon_pickup",
		sTexture = "powerup_bolt_yellow",
	},
	[4] = {
		type = PickupTypes.AddLife,
		soundfx = "collect_1",
		sTexture = "powerup_star_blue",
	},
})

-- Weighted random drop system (with chance of no drop)
DropTable = 
{
	{ item = PickupTypes.None, 				weight = 50 }, -- No drop, most common
	{ item = PickupTypes.Health, 			weight = 25 }, -- common
	{ item = PickupTypes.Shield, 			weight = 15 }, -- common
	{ item = PickupTypes.WeaponUpgrade, 	weight = 7 }, -- rare
	{ item = PickupTypes.AddLife, 			weight = 3 }, -- uncommon
}

-- chance that an enemy drops *anything at all*
local DROP_CHANCE = 0.3 -- 30% chance to drop, 70% no drop

-- Picks a random item from the drop table 
function GetRandomDrop()
	 -- First roll: does anything drop?
    if math.random() > DROP_CHANCE then
        return PickupTypes.None
    end
	
	-- Sum total weight
	local totalWeight = 0 
	for _, v in pairs(DropTable) do 
		totalWeight = totalWeight + v.weight
	end 
	
	-- Pick a random number in that range 
	local roll = math.random(1, totalWeight)
	
	-- Walk table until we find where the roll lands.
	local cumulative = 0
	for _, v in ipairs(DropTable) do 
		cumulative = cumulative + v.weight 
		if roll <= cumulative then 
			return v.item 
		end 
	end
end
