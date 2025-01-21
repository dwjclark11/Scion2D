
function CreateObject(obj)
	function obj:new(params)
		local this = params or {}

		setmetatable(this, self)
		self.__index = self 
		return this 
	end

	return obj
end

Subscriber = {}
function Subscriber:Create()
	function self:Update(param)
	end

	return CreateObject(Subscriber):new()
end 

Event = {} 
function Event:Create()
	self.m_Subscribers = {}

	function self:SubscribeToEvent(subscriber)
		table.insert(self.m_Subscribers, subscriber)
	end

	function self:Unsubscribe(subscriber)
		for k, v in pairs(self.m_Subscribers) do 
			if v == subscriber then 
				table.remove(self.m_Subscribers, k)
			end 
		end 
	end

	function self:Execute()
	end 

	return CreateObject(Event):new()
end

function Event:ResetSubscribers()
	for k, v in ipairs(self.m_Subscribers) do 
		self.m_Subscribers[k] = nil 
	end
end