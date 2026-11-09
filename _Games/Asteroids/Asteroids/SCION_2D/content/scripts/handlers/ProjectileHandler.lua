--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

-- Tracks all active projectiles, keyed by entity id
ProjectileHandler = S2D_Class("ProjectileHandler")

function ProjectileHandler:Init()
	self.projectiles = {}
	self.count = 0
	self.clock = Timer()
	self.clock:start()
end

function ProjectileHandler:NumProjectiles()
	return self.count
end

function ProjectileHandler:AddProjectile(projectile)
	projectile.expireTime = self.clock:elapsed_ms() + projectile.projectileLifetime
	self.projectiles[projectile:GetID()] = projectile
	self.count = self.count + 1
end

-- Removes projectiles that expired or left the window
function ProjectileHandler:Update()
	local now = self.clock:elapsed_ms()

	for id, projectile in pairs(self.projectiles) do
		if projectile:TimesUp(now) or not projectile:Update() then
			projectile:Destroy()
			self.projectiles[id] = nil
			self.count = self.count - 1
		end
	end
end

function ProjectileHandler:Reset()
	for id, projectile in pairs(self.projectiles) do
		projectile:Destroy()
		self.projectiles[id] = nil
	end

	self.count = 0
end

function ProjectileHandler:DestroyProjectile(projectile_id)
	local projectile = self.projectiles[projectile_id]
	if projectile then
		projectile:Destroy()
		self.projectiles[projectile_id] = nil
		self.count = self.count - 1
	end
end

gProjectileHandler = ProjectileHandler:Create()
