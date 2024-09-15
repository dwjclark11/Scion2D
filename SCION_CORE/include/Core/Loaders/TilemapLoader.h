#pragma once
#include <string>

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_CORE::Loaders
{
class TilemapLoader
{
  private:
	bool SaveTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile );
	bool LoadTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile );

	// TODO: Save and load functions for lua serializer

  public:
	TilemapLoader() = default;
	~TilemapLoader() = default;

	bool SaveTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false );
	bool LoadTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false );
};
} // namespace SCION_CORE::Loaders
