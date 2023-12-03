-------------------------------------------------------------------
-- Helper Functions
-------------------------------------------------------------------
--[[
	Takes in an entity definition and builds a new entity by
	adding the given components and values based on the provided 
	definition.
	Returns the newly created entity's ID
--]]
function LoadEntity(def)
	assert(def, "Def does not exist")

	local tag = ""
	if def.tag then
		tag = def.tag 
	end

	local group = ""
	if def.group then 
		group = def.group 
	end 

	local newEntity = Entity(tag, group)

	if def.components.transform then
		newEntity:add_component(
			Transform(
				vec2(
					def.components.transform.position.x,
					def.components.transform.position.y
				),
				vec2(
					def.components.transform.scale.x,
					def.components.transform.scale.y
				),
				def.components.transform.rotation
			)
		)
	end

	if def.components.sprite then
		local sprite = newEntity:add_component(
			Sprite(
				def.components.sprite.asset_name,
				def.components.sprite.width,
				def.components.sprite.height,
				def.components.sprite.start_x,
				def.components.sprite.start_y,
				def.components.sprite.layer
			)
		)
		sprite:generate_uvs()
		sprite.bHidden = def.components.sprite.bHidden or false
	end

	if def.components.circle_collider then
		newEntity:add_component(
			CircleCollider(
				def.components.circle_collider.radius
			)
		)
	end

	if def.components.animation then 
		newEntity:add_component(
			Animation(
				def.components.animation.num_frames,
				def.components.animation.frame_rate,
				def.components.animation.frame_offset,
				def.components.animation.bVertical,
				def.components.animation.bLooped
			)
		)
	end

	return newEntity:id()
end

-- Returns a random vec2 based on the min/max speed arguments
function GetRandomVelocity(min_speed, max_speed)
	return vec2(
		math.random(min_speed, max_speed),
		math.random(min_speed, max_speed)
	)
end

--[[
	Gets the value of the specific digit for 
	the number that is sent in. For instance,
	if the number is 9596 and we want digit three,
	it would return 5.
--]]
function GetDigit(num, digit)
	local n = 10 ^ digit 
	local n1 = 10 ^ (digit - 1)
	return math.floor((num % n) / n1)
end


--[[
	Tileset Lua class used for parsing necessary tileset info
	when loading maps created using the Tiled Map Editor.
--]]
Tileset = {}
Tileset.__index = Tileset
function Tileset:Create(params)
	local this = 
	{
		name = params.name, 
		columns = params.columns,
		width = params.width,
		height = params.height,
		tilewidth = params.tilewidth,
		tileheight = params.tileheight,
		firstgid = params.firstgid
	}

	this.rows = params.height / params.tileheight
	this.lastgid = math.floor(((this.rows * this.columns) + this.firstgid) - 1)

	setmetatable(this, self)
	return this
end

-- Checks to see if the given ID exists as a tile in the tileset.
-- returns true if the id is between the first and last gid
function Tileset:TileIdExists(id)
	return id >= self.firstgid and id <= self.lastgid
end

-- Gets the needed start_x and start_y values for the sprite component
-- When generating the uvs, the start x and start y are used to tell the 
-- engine where on the tilesheet the tile sprite is.
function Tileset:GetTileStartXY(id)
	assert(self:TileIdExists(id), "Tile ID[" ..id .."] does not exist in tileset[" ..self.name .."]")
	local actualTileID = id - self.firstgid
	local start_y = math.floor(actualTileID / self.columns)
	local start_x = math.floor(actualTileID % self.columns)
	return start_x, start_y
end

-- Loop through a tilesets table and check to see if that particular ID 
-- exists in a tileset. 
-- Returns the correct tileset if it exists, nil otherwise.
function GetTileset(tilesets, id)
	assert(tilesets)
	for k, v in pairs(tilesets) do 
		if v:TileIdExists(id) then 
			return v
		end 
	end 
	return nil
end

-- Takes in the Tilemap table returned from tilemaps created using Tiled Map Editor.
-- Creates a new tileset object for each tileset and then loads new tile entities based 
-- on their layer, tile id, and position.
function LoadMap(mapDef)
	assert(mapDef)
	local tilesets = {}
	for k, v in pairs(mapDef.tilesets) do 
		local newTileset = Tileset:Create(
			{
				name = v.name,
				columns = v.columns,
				width = v.imagewidth,
				height = v.imageheight,
				tilewidth = v.tilewidth,
				tileheight = v.tileheight,
				firstgid = v.firstgid
			}
		)

		table.insert(tilesets, newTileset)
	end

	-- Loop through the layers and create the tiles 
	for k, v in pairs(mapDef.layers) do 
		local rows = v.height - 1 
		local cols = v.width 
		local layer = k - 1 

		for row = 0, rows do
			for col = 1, cols do
				local id = v.data[row * cols + col]

				if id == 0 then 
					goto continue 
				end 
				
				local tileset = GetTileset(tilesets, id)
				assert(tileset, "Tileset does not exist with id: " ..id)
				local start_x, start_y = tileset:GetTileStartXY(id)

				-- Scale is used for testing, will change this once zoom in camera is working.
				local scale = 2 
				local position = vec2(((col - 1) * tileset.tilewidth * scale), (row * tileset.tileheight * scale))

				local tile = Entity("", "tiles")
				tile:add_component(Transform(position, vec2(scale, scale), 0))

				local sprite = tile:add_component(
					Sprite(
						tileset.name,
						tileset.tilewidth,
						tileset.tileheight,
						start_x, start_y,
						layer
					)
				)
				sprite:generate_uvs()

				-- Currently the box collider has a sprite, this will be changed once we can draw
				-- Simple Primitives in the engine.
				if tileset.name == "collider" then 
					tile:add_component(
						BoxCollider(
							tileset.tilewidth,
							tileset.tileheight,
							vec2(0, 0)
						)
					)

					-- Do we want to show/hide the collider sprite?
					--sprite.bHidden = true
				end

				::continue::
			end
		end
	end
end

--[[
	Loads all the assets into the AssetManager that are defined in the 
	given assets table. 
	Expects a table in the form of 
		Assets = {
			textures { ... },
			music { ... },
			sound_fx { ... },
			-- ...Other asset types as needed
		}
--]]
function LoadAssets(assets)
	for k, v in pairs(assets.textures) do
		if not AssetManager.add_texture(v.name, v.path, v.pixel_art) then
			print("Failed to load texture [" ..v.name .."] at path [" ..v.path .."]")
		else
			print("Loaded Texture [" ..v.name .."]")
		end
	end

	for k, v in pairs(assets.music) do 
		if not AssetManager.add_music(v.name, v.path) then 
			print("Failed to load music [" ..v.name .."] at path [" ..v.path .."]")
		else
			print("Loaded music [" ..v.name .."]")
		end
	end

	for k, v in pairs(assets.sound_fx) do
		if not AssetManager.add_soundfx(v.name, v.path) then
			print("Failed to load soundfx [" ..v.name .."] at path [" ..v.path .."]")
		else
			print("Loaded soundfx [" ..v.name .."]")
		end
	end
	-- TODO: Add other loading of assets as needed
end