--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]


--[[
@class ProjectileHandler
-----------------
@brief Manages all active projectiles in the game. 
Handles creation, updates, destruction, and reset of projectile entities.

Responsibilities:
- Track all active projectiles
- Update projectile state each frame
- Remove expired or invalid projectiles
- Destroy projectiles by ID
- Clear all projectiles when needed
]]
ProjectileHandler = S2D_Class("ProjectileHandler")

-----------------------------------------------------------------------------------------------------------------
-- @brief Initialize the projectile handler with an empty projectile list
-----------------------------------------------------------------------------------------------------------------
function ProjectileHandler:Init()
	self.projectiles = {}
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Get the number of active projectiles
-- @return Integer count of current projectiles
-----------------------------------------------------------------------------------------------------------------
function ProjectileHandler:NumProjectiles()
	return #self.projectiles 
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Add a new projectile to the handler
-- @param projectile The projectile object to register
-----------------------------------------------------------------------------------------------------------------
function ProjectileHandler:AddProjectile(projectile)
	table.insert(self.projectiles, projectile)
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Update all projectiles
-- Removes and destroys projectiles whose time has expired or that failed to update
-----------------------------------------------------------------------------------------------------------------
function ProjectileHandler:Update()
	for k, v in pairs(self.projectiles) do
		if v:TimesUp() or not v:Update() then 
			v:Destroy()
			self.projectiles[k] = nil
		end
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Reset the projectile handler
-- Destroys and clears all remaining projectiles
-----------------------------------------------------------------------------------------------------------------
function ProjectileHandler:Reset()
	for k, v in pairs(self.projectiles) do 
		v:Destroy()
		self.projectiles[k] = nil
	end
end

-----------------------------------------------------------------------------------------------------------------
-- @brief Destroy a specific projectile by ID
-- @param projectile_id The entity ID of the projectile to destroy
-----------------------------------------------------------------------------------------------------------------
function ProjectileHandler:DestroyProjectile(projectile_id)
	for k, v in pairs(self.projectiles) do 
		if v:GetID() == projectile_id then 
			v:Destroy()
			self.projectiles[k] = nil 
		end
	end
end

----------------------------------------------------------------------------------------------------------------
-- @brief Global instance of the ProjectileHandler
-----------------------------------------------------------------------------------------------------------------
gProjectileHandler = ProjectileHandler:Create()
