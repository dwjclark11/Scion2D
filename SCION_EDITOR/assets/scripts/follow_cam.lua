FollowCam = {}
FollowCam.__index = FollowCam

function FollowCam:Create(cam, params)
	local this = 
	{
		m_Cam = cam or Camera.get(),
		m_Scale = params.scale or 2,
		m_MinX = params.min_x or 0,
		m_MinY = params.min_y or 0,
		m_MaxX = params.max_x * (params.scale or 2) or cam.width() * 2,
		m_MaxY = params.max_y * (params.scale or 2) or cam.height() * 2,
		m_Springback = params.springback or 1.0
	}
	
	this.m_Cam.set_position(vec2(this.m_MinX, this.m_MinY))
	this.m_Cam.set_scale(this.m_Scale)
	setmetatable(this, self)
	return this
end

function FollowCam:Update(entity_id)
	local entity = Entity(entity_id)
	local transform = entity:get_component(Transform)

	local cam_pos = self.m_Cam.position()
	local cam_width = self.m_Cam.width()
	local cam_height = self.m_Cam.height()

	local new_cam_pos = vec2(0, 0)

	new_cam_pos.x = (transform.position.x - cam_width / (2 * self.m_Scale)) * self.m_Scale
	new_cam_pos.y = (transform.position.y - cam_height / (2 * self.m_Scale)) * self.m_Scale

	-- Clamp camera position to min/max
	new_cam_pos.x = clamp(new_cam_pos.x, self.m_MinX, self.m_MaxX)
	new_cam_pos.y = clamp(new_cam_pos.y, self.m_MinY, self.m_MaxY)

	self.m_Cam.set_position(
		vec2(
			lerp(cam_pos.x, new_cam_pos.x, self.m_Springback),
			lerp(cam_pos.y, new_cam_pos.y, self.m_Springback)
		)
	)

end