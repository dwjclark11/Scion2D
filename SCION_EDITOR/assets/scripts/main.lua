-- Main Lua Script!
run_script("assets/scripts/Platformer/defs/assetDefs.lua")
run_script("assets/scripts/Platformer/maps/test_platformer.lua")
run_script("assets/scripts/Platformer/utilities/utilities.lua")
run_script("assets/scripts/Platformer/utilities/rain_generator.lua")
run_script("assets/scripts/Platformer/utilities/scene_dimmer.lua")
run_script("assets/scripts/Platformer/events/event_manager.lua")
run_script("assets/scripts/Platformer/events/collision_event.lua")
run_script("assets/scripts/Platformer/systems/trigger_system.lua")

run_script("assets/scripts/Platformer/game_states/title_state.lua")
run_script("assets/scripts/Platformer/game_states/game_state.lua")

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

main = {
	[1] = {
		update = function()
			gStateStack:update(0.016)
		end
	},
	[2] = {
		render = function()
			gStateStack:render()
		end
	},
}
