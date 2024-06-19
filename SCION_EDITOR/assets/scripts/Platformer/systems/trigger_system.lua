TriggerSystem = Subscriber:Create()

function TriggerSystem:Create()
	return CreateObject(TriggerSystem):new()
end 

g_DebounceTimer = Timer()

function TriggerSystem:OnCollision(object_a, object_b)
	if object_a.tag == "player" then 
		if object_b.group == "hole_triggers" and not self:Debounce() then
			self:OnPlayerTriggered(object_b, object_a)
		end 

		for _, v in pairs(object_a.contactEntities) do 
			if v.group == "hole_triggers" and not self:Debounce() then 
				self:OnPlayerTriggered(v, object_a)
			elseif v.group == "ground" then 
				self:OnPlayerGrounded(object_a)
			end 
		end 
	elseif object_b.tag == "player" then
		if object_a.group == "hole_triggers" and not self:Debounce() then
			self:OnPlayerTriggered(object_a, object_b)
		end 

		for _, v in pairs(object_b.contactEntities) do 
			if v.group == "hole_triggers" and not self:Debounce() then 
				self:OnPlayerTriggered(v, object_b)
			elseif v.group == "ground" then 
				self:OnPlayerGrounded(object_b)
			end 
		end
	end 
end

function TriggerSystem:Debounce() 
	return g_DebounceTimer:is_running()
end 

function TriggerSystem:Update()
	if g_DebounceTimer:is_running() and g_DebounceTimer:elapsed_ms() > 100 then 
		g_DebounceTimer:stop()
	end 
end

function TriggerSystem:OnPlayerTriggered(trigger, player)
	S2D_log("%s has been activated by %s", trigger.group, player.tag)
	g_DebounceTimer:start()
	local playerEntity = Entity(player.entityID)
	local physics = playerEntity:get_component(PhysicsComp)
	physics:set_transform(vec2(16, 416))
	physics:linear_impulse(vec2(0, 5))
end

function TriggerSystem:OnPlayerGrounded(player)
	local playerChar = GetActiveCharacter(player.entityID)
	assert(playerChar, "Failed to get the player character")
	playerChar.m_bGrounded = true
end
