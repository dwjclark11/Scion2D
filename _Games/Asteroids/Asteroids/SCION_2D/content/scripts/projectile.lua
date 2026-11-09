--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-- Projectile fired by a ship. Position and radius are tracked in Lua numbers so
-- the collision system never has to read components. The lifetime is checked by
-- the ProjectileHandler against its shared clock (see AddProjectile).
Projectile = S2D_Class("Projectile")

-- params: { def, dir, start_pos, rotation, color }
function Projectile:Init(params)
	params = params or {}

	self.def = params.def
	self.dir = params.dir
	self.startPos = params.start_pos
	self.rotation = params.rotation

	local projectile = ProjectileDefs[self.def]
	assert(projectile)

	local sprite = projectile.components.sprite

	self.projectileLifetime = projectile.life_time or 2000
	self.speed = projectile.proj_speed or 10
	self.expireTime = 0
	self.vx = self.dir.x * self.speed
	self.vy = self.dir.y * self.speed
	self.halfWidth = sprite.width * 0.5
	self.halfHeight = sprite.height * 0.5
	self.radius = projectile.components.circle_collider.radius
	self.x = self.startPos.x
	self.y = self.startPos.y
	self.cx = self.x + self.halfWidth
	self.cy = self.y + self.halfHeight

	self.entityID = LoadEntity(projectile)
	self.entity = Entity(self.entityID)

	local transform = self.entity:getComponent(Transform)
	transform.position = self.startPos
	transform.rotation = self.rotation

	if params.color then
		local entitySprite = self.entity:getComponent(Sprite)
		entitySprite.color = params.color
	end
end

-- Returns false when the projectile leaves the window
function Projectile:Update()
	local x = self.x + self.vx
	local y = self.y + self.vy

	self.x = x
	self.y = y
	self.cx = x + self.halfWidth
	self.cy = y + self.halfHeight

	local position = self.entity:getComponent(Transform).position
	position.x = x
	position.y = y

	if x < 0 or x > WINDOW_WIDTH or y < 0 or y > WINDOW_HEIGHT then
		return false
	end

	return true
end

function Projectile:GetID()
	return self.entityID
end

-- now is the handler clock time in ms
function Projectile:TimesUp(now)
	return now >= self.expireTime
end

function Projectile:Destroy()
	self.entity:destroy()
end
