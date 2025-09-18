--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

main = { 
	[1] = {
		init = function()
			math.randomseed(os.time())
			gStateStack = StateStack()
			local title = TitleState:Create( { stack = gStateStack } )
			gStateStack:changeState(title:GetState())
			Music.setVolume(20)
		end
	},
	[2] = { 
		update = function() 
			if gStateStack then
				gStateStack:update(S2D_DeltaTime())
			end
		end
	}, 
	[3] = { 
		render = function() end
	}, 
}
