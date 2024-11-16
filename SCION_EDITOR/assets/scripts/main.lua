-- Main Lua Script!
--[[
S2D_run_script("assets/scripts/Platformer/script_list.lua")

S2D_load_script_table(ScriptList)

LoadAssets(AssetDefs)
gFollowCam = nil 
gCollisionEvent = nil 
gTriggerSystem = nil 
gPlayer = nil 
gMainCam = Camera.get()
gMainCam.set_scale(2.0)

gStateStack = StateStack()
local title = TitleState:Create(gStateStack)
gStateStack:change_state(title)
--]]
main = {
	[1] = {
		update = function()
			--gStateStack:update(0.016)
		end
	},
	[2] = {
		render = function()
			--gStateStack:render()
		end
	},
}
