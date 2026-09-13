#pragma once
#include <rapidjson/document.h>

namespace Scion::Filesystem
{
class JSONSerializer;
}

namespace Scion::Editor
{
struct SceneFolder
{
	using Id = uint32_t;
	static constexpr Id NoParent = 0;
	static constexpr Id NullId = 0;

	Id id{ NullId };
	Id parentId{ NoParent }; // 0 = root level folder

	std::string name{};
	/** @brief Used in scene hierarchy to show all children. */
	bool bExpanded{ true };
	/** @brief Used in scene hierarchy to open the node if folder/entity is added. */
	bool bPendingOpen{ false };

	std::unordered_set<std::string> entityNames{};

	bool ContainsByName( const std::string& name ) const { return entityNames.contains( name ); }
	void AddByName( const std::string& name ) { entityNames.insert( name ); }
	void RemoveByName( const std::string& name ) { entityNames.erase( name ); }
};

class SceneFolderManager
{
  public:
	SceneFolderManager() = default;

	// -- Folder CRUD
	SceneFolder& CreateFolder( const std::string& name = "New Folder",
							   SceneFolder::Id parentId = SceneFolder::NoParent );

	bool DeleteFolder( SceneFolder::Id id );
	SceneFolder* GetFolder( SceneFolder::Id id );
	const SceneFolder* GetFolder( SceneFolder::Id id ) const;

	/** @brief Returns direct children folders of the given parent (0 == root). */
	std::vector<SceneFolder*> GetChildFolders( SceneFolder::Id parentId );

	/** @brief Moves a folder under a new parent. Rejects cycles. */
	bool MoveFolderToParent( SceneFolder::Id id, SceneFolder::Id newParentId );

	// -- Entity Membership - All by name

	/** @brief Returns the folder that owns this entity name, or nullptr. */
	SceneFolder* FindFolderForEntity( const std::string& entityName );

	/** @brief Moves entity into a folder. Removes from any prior folder first.
	 * Pass targetId = NullId to just remove from all folders.
	 */
	void MoveEntityToFolder( const std::string& entityName, SceneFolder::Id targetId );

	/** @brief Called when an entity is deleted. Removes from all folders. */
	void OnEntityDeleted( const std::string& entityName );

	/** @brief Called when an entity is renamed. Migrates membership. */
	void OnEntityRenamed( const std::string& oldName, const std::string& newName );

	const std::unordered_map<SceneFolder::Id, SceneFolder>& GetFolders() const { return m_mapFolders; }
	std::unordered_map<SceneFolder::Id, SceneFolder>& GetFolders() { return m_mapFolders; }

	/** @brief Check to see if the folder name exists in the current scene. */
	bool FolderExists( const std::string& folderName ) const;

	// -- Serialization - Called by owning SceneObject
	void SaveToJSON( Scion::Filesystem::JSONSerializer& serializer ) const;
	void LoadFromJSON( const rapidjson::Value& foldersArray );

  private:
	bool IsAncestor( SceneFolder::Id potentialAncestor, SceneFolder::Id ofId ) const;

  private:
	std::unordered_map<SceneFolder::Id, SceneFolder> m_mapFolders{};
	SceneFolder::Id m_NextId{ 1 };
};


} // namespace Scion::Editor
