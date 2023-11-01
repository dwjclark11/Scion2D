Ship = {}
Ship.__index = Ship

function Ship:Create(def)
	local this = 
	{
		m_EntityID = def.id,
		m_ForwardSpeed = def.fwd_speed or 10,
		m_AngularSpeed = def.ang_speed or 4,
		m_DriftSpeed = def.drift_speed or 1,
		m_DriftAngle = vec2(
			math.cos(0),
			math.sin(0)
		)
	}

	setmetatable(this, self)
	return this
end

function Ship:UpdateShip()
	local ship = Entity(self.m_EntityID)
	local transform = ship:get_component(Transform)
	local sprite = ship:get_component(Sprite)

	local forward = vec2(
		math.cos(math.rad(transform.rotation)), -- x Value
		math.sin(math.rad(transform.rotation)) -- y Value
	)
	-- CCW Rotation
	if Keyboard.pressed(KEY_A) then
		transform.rotation = transform.rotation - self.m_AngularSpeed
	elseif Keyboard.pressed(KEY_D) then -- CW
		transform.rotation = transform.rotation + self.m_AngularSpeed
	end

	if Keyboard.pressed(KEY_W) then 
		transform.position = transform.position + forward * self.m_ForwardSpeed
		self.m_DriftAngle = forward
	else
		transform.position = transform.position + self.m_DriftAngle * self.m_DriftSpeed
	end

	CheckPos(transform.position, sprite.width, sprite.height)
end