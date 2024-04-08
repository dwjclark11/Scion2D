TriggerSystem = Subscriber:Create()

function TriggerSystem:Create()
	return CreateObject(TriggerSystem):new()
end 

function TriggerSystem:OnCollision(object_a, object_b)
	if object_a.bTrigger and object_b.tag == "player" then 
		self:OnPlayerTriggered(object_a, object_b)
	end 

	if object_b.bTrigger and object_a.tag == "player" then 
		self:OnPlayerTriggered(object_b, object_a)
	end 
end

function TriggerSystem:OnPlayerTriggered(trigger, player)
	print(trigger.group .. "has been activated by " ..player.tag)
end