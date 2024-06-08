CollisionEvent = Event:Create() 

function CollisionEvent:Create()
	local object_a = nil 
	local object_b = nil 

	function self:Execute()
		for k, v in pairs(self.m_Subscribers) do 
			v:OnCollision(object_a, object_b)
		end
	end

	function self:EmitEvent(obj_a, obj_b)
		object_a = obj_a 
		object_b = obj_b 
		self:Execute()
	end

	return CreateObject(CollisionEvent):new()
end