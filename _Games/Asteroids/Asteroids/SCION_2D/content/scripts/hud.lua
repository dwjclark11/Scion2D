--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-----------------------------------------------------------------------------------------------------------------
-- @class Hud (Heads-Up Display)
-- 
-- @brief A Lua class that manages the game's UI overlay including score, 
-- player lives, shield/health bars, and damage effects. 
-- It also handles events from ships (health, lives, shields).
-----------------------------------------------------------------------------------------------------------------
Hud = S2D_Class("Hud")

-----------------------------------------------------------------------------------------------------------------
-- @brief Initializes the HUD by setting up lives indicators, score display,
-- health/shield bars, and event handlers for ship updates.
-----------------------------------------------------------------------------------------------------------------
function Hud:Init()
	
	self.lives = {}
	self.score = 0

	-- Damage screen overlay setup
	self.damageScreen = Entity("damaged_screen", "")
	self.damageScreen:addComponent(UIComp())
	self.damageScreen:addComponent(Transform(vec2(0, 0), vec2(1, 1), 0.0))
	local damageSprite = self.damageScreen:addComponent(Sprite("screen_crack", 640, 480, 0, 0, 1))
	damageSprite:generateUVs()
	damageSprite.bHidden = true
	
	local reg = Registry()
	
	-- Create out Lives Entities
	for i = 1, 6 do
		local livesEntity = reg:findEntityByTag("Life_" ..tostring(i))
		table.insert(self.lives, livesEntity)
		
		-- Hide player 2 lives -- TODO: add a selector to the start to choose 1 or 2 players
		if i > 3 then 
			local sprite = livesEntity:getComponent(Sprite)
			sprite.bHidden = true 
		end		
	end

	-- Score entity data
	self.scoreLabel = reg:findEntityByTag("ScoreLabel")
	self.scoreValue = reg:findEntityByTag("ScoreValue")
	
	-- Shield bar setup
	self.shieldBar = reg:findEntityByTag("ShieldBar")
	self.shieldBarFill = reg:findEntityByTag("ShieldBarFill")
	self.shieldBarImage = reg:findEntityByTag("ShieldBarImage")
	local shieldFillTransform = self.shieldBarFill:getComponent(Transform)
	self.shieldFillOriginalScale = shieldFillTransform.scale

	-- Health bar setup
	self.lifeBar = reg:findEntityByTag("LifeBar")
	self.lifeBarFill = reg:findEntityByTag("LifeBarFill")
	self.lifeBarImage = reg:findEntityByTag("LifeBarImage")
	local lifeFillTransform = self.lifeBarFill:getComponent(Transform)
	local lifeScaleX =  lifeFillTransform.scale.x
	self.lifeFillScaleX = lifeScaleX

	 -- Initialize score display
	local text = self.scoreValue:getComponent(TextComponent)
	text.textStr = "0"

	-- Internal state
	self.bGameOver = false 
	self.shipEventHandler = LuaEventHandler( function (event) self:HandleShipEvent(event) end )
	gShipHandler:AddEventHandler(self.shipEventHandler)

	-- Timers for flashing/damage effects
	self.shipDamageTimer = Timer()
	self.shieldDamageTimer = Timer()
	self.flashTimer = Timer()
	self.bShip1Damaged = false	
	self.bShield1Damaged = false	
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates HUD state every frame. Handles score and damage effects.
-----------------------------------------------------------------------------------------------------------------
function Hud:Update()
	if self.bGameOver then
		return 
	end
	
	self:UpdateScore()
	self:UpdateShipDamage()
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Toggles the damage screen visibility.
-- @param bShow (bool) - true to show, false to hide
-----------------------------------------------------------------------------------------------------------------
function Hud:ShowDamage(bShow)
	local sprite = self.damageScreen:getComponent(Sprite)
	sprite.bHidden = not bShow
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates player life indicators based on ship lives.
-- @param event (LuaEvent) - contains shipID to update lives from
-----------------------------------------------------------------------------------------------------------------
function Hud:UpdateLives(event)
	local ship = gShipHandler:GetShipByID(event.data.event.shipID)
	if ship then 
		local numLives = ship:GetLives()
		for i = 1, 3 do
			local sprite = self.lives[i]:getComponent(Sprite)
			if i <= numLives then
				sprite.bHidden = false
			else
				sprite.bHidden = true
			end
		end
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates the life bar UI when the ship takes damage.
-- @param event (LuaEvent) - contains shipID for health update
-----------------------------------------------------------------------------------------------------------------
function Hud:UpdateHealth(event)
	local ship = gShipHandler:GetShipByID(event.data.event.shipID)
	if not ship then 
		S2D_warn("Failed to update health. Ship [%d] is invalid", event.data.event.shipID)
		return 
	end
	
	local currentHealth = ship:GetHealth() 
	local healthPct = currentHealth / 100 
	
	local healthFillTransform = self.lifeBarFill:getComponent(Transform)
	healthFillTransform.scale.x = S2D_clamp(self.lifeFillScaleX * healthPct, 0.0, self.lifeFillScaleX)
	
	self.bShip1Damaged = true 
	self.shipDamageTimer:start()
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates the shield bar UI when shield takes damage.
-- @param event (LuaEvent) - contains shipID for shield update
-----------------------------------------------------------------------------------------------------------------
function Hud:UpdateShield(event)
	local ship = gShipHandler:GetShipByID(event.data.event.shipID)
	if not ship then 
		S2D_warn("Failed to update Shield. Ship [%d] is invalid", event.data.event.shipID)
		return 
	end
	
	local currentShield = ship:GetShieldHealth() 
	local shieldPct = currentShield / 100 
	
	local shieldFillTransform = self.shieldBarFill:getComponent(Transform)
	shieldFillTransform.scale.x = S2D_clamp(self.lifeFillScaleX * shieldPct, 0.0, self.lifeFillScaleX)
	self.bShield1Damaged = true 
	self.shieldDamageTimer:start()
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates score if it has changed since last frame.
-----------------------------------------------------------------------------------------------------------------
function Hud:UpdateScore()
	local current_score = gData:GetScore()
	if current_score == self.score then	
		return 
	end 
	self:SetScore(current_score)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Sets the displayed score.
-- @param current_score (number) - the new score value
-----------------------------------------------------------------------------------------------------------------
function Hud:SetScore(current_score)
	-- Set the Score
	self.score = current_score
	local text = self.scoreValue:getComponent(TextComponent)
	text.textStr = tostring(math.floor(self.score))
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Resets the HUD state for a new game session.
-----------------------------------------------------------------------------------------------------------------
function Hud:Reset()
	self.bGameOver = false 
	self.bShowGameOver = false 
	self:SetScore(0)
	self:ShowDamage(false)
	self.bShip1Damaged = false 
	self.shipDamageTimer:stop()
	self.flashTimer:stop()
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Handles ship-related events (health, lives, shield).
-- @param event (LuaEvent) - contains event type and shipID
-----------------------------------------------------------------------------------------------------------------
function Hud:HandleShipEvent(event)
	S2D_switch(event.data.type, 
		{
			[1] = function() self:UpdateHealth(event) end,
			[2] = function() self:UpdateLives(event) end,
			[3] = function() self:UpdateShield(event) end,
		},
		function() S2D_warn("Not a valid case!") end
	)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Updates flashing animations for damaged ship or shield.
-- Uses timers to flash between two colors for 500ms.
-----------------------------------------------------------------------------------------------------------------
function Hud:UpdateShipDamage()
	if self.bShip1Damaged and self.shipDamageTimer:is_running() then 
		local sprite = self.lifeBarFill:getComponent(Sprite)
		FlashColor(sprite, self.flashTimer, 50, Color(255, 0, 0, 200), Color(255, 255, 255, 200))
		
		if self.shipDamageTimer:elapsed_ms() > 500 then 
			self.bShip1Damaged = false 
			self.shipDamageTimer:stop()
			self.flashTimer:stop()
			sprite.color = Color(255, 255, 255, 255)
		end
	elseif self.bShield1Damaged and self.shieldDamageTimer:is_running() then  
		local sprite = self.shieldBarFill:getComponent(Sprite)
		FlashColor(sprite, self.flashTimer, 50, Color(0, 0, 255, 200), Color(255, 255, 255, 200))
		
		if self.shieldDamageTimer:elapsed_ms() > 500 then 
			self.bShield1Damaged = false 
			self.shieldDamageTimer:stop()
			self.flashTimer:stop()
			sprite.color = Color(255, 255, 255, 255)
		end
	end 
end
