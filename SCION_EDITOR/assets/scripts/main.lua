-- Main Lua Script!
--run_script("assets/scripts/TestProject/assetDefs.lua")
--run_script("assets/scripts/TestProject/testmap.lua")
--run_script("assets/scripts/utilities.lua")
--
--local tilemap = CreateTestMap()
--assert(tilemap)
--LoadAssets(AssetDefs)
--LoadMap(tilemap)

main = {
	[1] = {
		update = function()

		end
	},
	[2] = {
		render = function()
				--DrawLine(Line(vec2(50, 50), vec2(200, 200), Color(255, 0, 0, 255 )))
				--DrawLine(Line(vec2(200, 50), vec2(50, 200), Color(0, 255, 0, 255 )))
				--DrawRect(Rect(vec2(300, 300), 100, 100, Color(0, 0, 255, 255)))
				--DrawRect(Rect(vec2(300, 300), 100, 100, Color(0, 0, 255, 255)))
				--DrawCircle(vec2(200, 200), 1.0, 200.0, Color(0, 0, 255, 255))
				local text = "Text Batch Rendering " ..
							 "Part#3 - Wrapping the Text"

				DrawText(Text(vec2( 0.0, 300.0), 
					text,
					"pixel",
					640.0,
					Color(255, 0, 0, 255)
					)
				)

		end
	},
}