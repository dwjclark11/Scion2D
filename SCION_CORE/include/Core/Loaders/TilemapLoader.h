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
  public:
	TilemapLoader() = default;
	~TilemapLoader() = default;

	bool SaveTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false );
	bool LoadTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false );
	bool LoadGameObjects( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile, bool bUseJSON = false );
	bool SaveGameObjects( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile, bool bUseJSON = false );

private:
	bool SaveTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile );
	bool LoadTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile );

	bool SaveObjectMapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile );
	bool LoadObjectMapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile );

	// TODO: Save and load functions for lua serializer
};

/*
* We cannot just save the entity IDs for relationships since they are not guaranteed to be the same
* by entt; however, we can try to guarantee for each scene that the entity has a unique tag.
*/
struct SaveRelationship
{
	//entt::entity self{ entt::null };
	std::string sParent{ "" };
	std::string sNextSibling{ "" };
	std::string sPrevSibling{ "" };
	std::string sFirstChild{ "" };
};

} // namespace SCION_CORE::Loaders
