-- Main Lua Script!
-- [[
S2D_RunScript("assets/scripts/Platformer/script_list.lua")

S2D_LoadScriptTable(ScriptList)

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
			if gStateStack then
				gStateStack:update(0.016)
			end
		end
	},
	[2] = {
		render = function()
			if gStateStack then 
				gStateStack:render()
			end
		end
	},
}
