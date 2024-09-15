#include "Core/Loaders/TilemapLoader.h"

namespace SCION_CORE::Loaders
{
bool TilemapLoader::SaveTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile )
{
	/*
	* TODO:
	* 1) Create json serializer for the new tilemap path
	* 2) Start the document
	* 3) Start new array -- name it tilemap
	* 4) Loop through all of the tiles from the registry.
	* 5) Inside Loop - Start nameless object, start components object
	* 6) Serialze all the necessary components.
	* 7) End components table
	* 8) End the tile object table
	* 9) End array and finish document
	*/
	return false;
}
bool TilemapLoader::LoadTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile )
{
	return false;
}

bool TilemapLoader::SaveTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON )
{
	if ( bUseJSON )
		return SaveTilemapJSON( registry, sTilemapFile );

	return false;
}
bool TilemapLoader::LoadTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON )
{
	if ( bUseJSON )
		return LoadTilemapJSON( registry, sTilemapFile );

	return false;
}

} // namespace SCION_CORE::Loaders
