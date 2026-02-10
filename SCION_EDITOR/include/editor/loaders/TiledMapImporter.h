#pragma once
#include <glm/glm.hpp>

namespace Scion::Editor
{

class EditorSceneManager;

class TiledMapImporter
{
  public:
	TiledMapImporter() = delete;

	/*
	 * @brief Imports tilemaps made in the Tiled Map Editor and converts
	 * them to a new Scion Map and Scene.
	 * @param pSceneManager a pointer to the scene manager to create a new scene.
	 * @param sTileMapFile the path to the map filed to convert.
	 * @return Returns true if successful, false otherwise.
	 */
	static bool ImportTilemapFromTiled( EditorSceneManager* pSceneManager, const std::string sTiledMapFile );

  private:
	struct Tileset;
	static bool ImportFromTMXFile( EditorSceneManager* pSceneManager, const std::string sTiledMapFile );
	static bool ImportFromLuaFile( EditorSceneManager* pSceneManager, const std::string sTiledMapFile );
	static Tileset* GetTileset( std::vector<Tileset>& tilesets, int id );

  private:
	struct TileObject
	{
		std::string sName{};
		std::string sType{};
		glm::vec2 position{ 0.f };
		float width{ 0.f };
		float height{ 0.f };
		float rotation{ 0.f };
	};

	struct Tile
	{
		int id{ -1 };
		std::vector<TileObject> tileObjects;
	};

	struct Tileset
	{
		std::string sName{};
		int columns{ 1 };
		int rows{ 1 };
		int width{ 1 };
		int height{ 1 };
		int tileWidth{ 16 };
		int tileHeight{ 16 };
		int firstGID{ 1 };
		int tileCount{ 1 };
		std::vector<Tile> tiles{};

		inline void SetRows() { rows = height / tileHeight; }
		inline const bool TileIdExists( int id ) const { return id >= firstGID && id <= tileCount; }
		std::tuple<int, int> GetTileStartXY( int id );
		TileObject* GetObjectFromId( int id );
	};
};

} // namespace Scion::Editor
