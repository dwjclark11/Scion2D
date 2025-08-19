#pragma once

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

	/**
	 * @brief Saves tilemap data using the specified serialization format.
	 *
	 * Currently supports saving in JSON format only.
	 *
	 * @param registry        The ECS registry containing tile entities.
	 * @param sTilemapFile    The destination file path for the tilemap.
	 * @param bUseJSON        Flag indicating whether to use JSON format.
	 * @return true if the tilemap was saved successfully, false otherwise.
	 */
	bool SaveTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false );

	/**
	 * @brief Loads tilemap data using the specified serialization format.
	 *
	 * Currently supports loading from JSON format only.
	 *
	 * @param registry        The ECS registry to populate with tile entities.
	 * @param sTilemapFile    The source file path of the tilemap.
	 * @param bUseJSON        Flag indicating whether to use JSON format.
	 * @return true if the tilemap was loaded successfully, false otherwise.
	 */
	bool LoadTilemap( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile, bool bUseJSON = false );

	/**
	 * @brief Loads game object data using the specified serialization format.
	 *
	 * Currently supports loading from JSON format only.
	 *
	 * @param registry          The ECS registry to populate with game object entities.
	 * @param sObjectMapFile    The source file path of the game objects.
	 * @param bUseJSON          Flag indicating whether to use JSON format.
	 * @return true if the game objects were loaded successfully, false otherwise.
	 */
	bool LoadGameObjects( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile,
						  bool bUseJSON = false );

	/**
	 * @brief Saves game object data using the specified serialization format.
	 *
	 * Currently supports saving in JSON format only.
	 *
	 * @param registry          The ECS registry containing game objects.
	 * @param sObjectMapFile    The destination file path for the game objects.
	 * @param bUseJSON          Flag indicating whether to use JSON format.
	 * @return true if the game objects were saved successfully, false otherwise.
	 */
	bool SaveGameObjects( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile,
						  bool bUseJSON = false );

  private:
	/**
	 * @brief Serializes all tile entities from the ECS registry to a JSON tilemap file.
	 *
	 * Iterates over tile entities in the registry and serializes their components to a JSON file.
	 * Components include Transform, Sprite, and optional colliders, animation, and physics.
	 *
	 * @param registry        The ECS registry containing tile entities.
	 * @param sTilemapFile    The destination JSON file path to save the tilemap data.
	 * @return true if the tilemap was saved successfully, false otherwise.
	 */
	bool SaveTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile );

	/**
	 * @brief Deserializes tile entities from a JSON tilemap file into the ECS registry.
	 *
	 * Reads tile data from the specified JSON file and reconstructs each tile entity
	 * with its respective components in the ECS registry.
	 *
	 * @param registry        The ECS registry where tile entities will be created.
	 * @param sTilemapFile    The source JSON file path containing the tilemap data.
	 * @return true if the tilemap was loaded successfully, false otherwise.
	 */
	bool LoadTilemapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile );

	/**
	 * @brief Serializes all non-tile, non-uneditable game objects from the ECS registry to a JSON file.
	 *
	 * Serializes components such as Transform, Sprite, Collider, Animation, Physics, Text, UI, and Relationship.
	 * The file represents the game object layout and hierarchy for scene reconstruction.
	 *
	 * @param registry          The ECS registry containing game objects.
	 * @param sObjectMapFile    The destination JSON file path to save the game objects.
	 * @return true if the game objects were saved successfully, false otherwise.
	 */
	bool SaveObjectMapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile );

	/**
	 * @brief Deserializes game objects from a JSON file into the ECS registry, including hierarchy.
	 *
	 * Reconstructs entities with their components and re-links parent/child/sibling relationships.
	 * Supports components like Transform, Sprite, Collider, Animation, Physics, Text, UI, and Identification.
	 *
	 * @param registry          The ECS registry where game objects will be created.
	 * @param sObjectMapFile    The source JSON file path containing the game object data.
	 * @return true if game objects were loaded successfully, false otherwise.
	 */
	bool LoadObjectMapJSON( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile );

	// Save and load functions for lua serializer
	bool SaveTilemapLua( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile );
	bool LoadTilemapLua( SCION_CORE::ECS::Registry& registry, const std::string& sTilemapFile );

	bool SaveObjectMapLua( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile );
	bool LoadObjectMapLua( SCION_CORE::ECS::Registry& registry, const std::string& sObjectMapFile );
};

/*
 * We cannot just save the entity IDs for relationships since they are not guaranteed to be the same
 * by entt; however, we can try to guarantee for each scene that the entity has a unique tag.
 */
struct SaveRelationship
{
	// entt::entity self{ entt::null };
	std::string sParent{};
	std::string sNextSibling{};
	std::string sPrevSibling{};
	std::string sFirstChild{};
};

} // namespace SCION_CORE::Loaders
