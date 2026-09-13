#include "editor/scene/SceneFolderManager.h"
#include "ScionFilesystem/Serializers/JSONSerializer.h"

#include "Logger/Logger.h"

namespace Scion::Editor
{

SceneFolder& SceneFolderManager::CreateFolder( const std::string& name, SceneFolder::Id parentId )
{
	SceneFolder::Id id{ m_NextId++ };
	auto [ itr, inserted ] = m_mapFolders.emplace( id, SceneFolder{ .id = id } );
	SceneFolder& folder = itr->second;

	int currentID{ 1 };
	std::string sCopy{ name };
	if ( FolderExists( sCopy ) )
	{
		sCopy = fmt::format( "{}_{}", sCopy, currentID++ );
		while ( FolderExists( sCopy ) )
		{
			sCopy = fmt::format( "{}_{}", sCopy, currentID++ );
		}
	}

	folder.name = std::move( sCopy );
	folder.parentId = parentId;
	return folder;
}

bool SceneFolderManager::DeleteFolder( SceneFolder::Id id )
{
	// Re-parent children to the delete folder's parent before removing
	SceneFolder::Id grandParentId = SceneFolder::NoParent;
	if ( const auto* pFolder = GetFolder( id ) )
	{
		grandParentId = pFolder->parentId;
	}

	for ( auto& folder : m_mapFolders )
	{
		if ( folder.second.parentId == id )
		{
			folder.second.parentId = grandParentId;
		}
	}

	// Find the folder to delete
	auto it = m_mapFolders.find( id );
	if ( it == m_mapFolders.end() )
		return false;

	m_mapFolders.erase( it );

	return true;
}

SceneFolder* SceneFolderManager::GetFolder( SceneFolder::Id id )
{
	auto it = m_mapFolders.find( id );
	return it != m_mapFolders.end() ? &it->second : nullptr;
}

const SceneFolder* SceneFolderManager::GetFolder( SceneFolder::Id id ) const
{
	auto it = m_mapFolders.find( id );
	return it != m_mapFolders.end() ? &it->second : nullptr;
}

std::vector<SceneFolder*> SceneFolderManager::GetChildFolders( SceneFolder::Id parentId )
{
	std::vector<SceneFolder*> children{};
	for ( auto& folder : m_mapFolders )
	{
		if ( folder.second.parentId == parentId )
		{
			children.push_back( &folder.second );
		}
	}

	return children;
}

bool SceneFolderManager::MoveFolderToParent( SceneFolder::Id id, SceneFolder::Id newParentId )
{
	// Reject self-parenting
	if ( id == newParentId )
		return false;

	// Reject cycles - newParentId must not be a descendent of id
	if ( newParentId != SceneFolder::NoParent && IsAncestor( id, newParentId ) )
	{
		SCION_ERROR( "Cannot move folder [{}] - Would create a cycle.", id );
		return false;
	}

	if ( auto* pFolder = GetFolder( id ) )
	{
		pFolder->parentId = newParentId;
		return true;
	}

	return false;
}

SceneFolder* SceneFolderManager::FindFolderForEntity( const std::string& entityName )
{
	for ( auto& [ _, folder ] : m_mapFolders )
	{
		if ( folder.ContainsByName( entityName ) )
		{
			return &folder;
		}
	}

	return nullptr;
}

void SceneFolderManager::MoveEntityToFolder( const std::string& entityName, SceneFolder::Id targetId )
{
	// Remove entity from other folders
	for ( auto& [ _, folder ] : m_mapFolders )
	{
		folder.RemoveByName( entityName );
	}

	// Check to see if the targer folder is no folder
	if ( targetId == SceneFolder::NullId )
		return;

	// Get the target folder and add by name
	if ( auto* pTarget = GetFolder( targetId ) )
	{
		pTarget->AddByName( entityName );
	}
	else
	{
		SCION_ERROR( "Folder ID[{}] not found.", targetId );
	}
}

void SceneFolderManager::OnEntityDeleted( const std::string& entityName )
{
	for ( auto& [ _, folder ] : m_mapFolders )
	{
		folder.RemoveByName( entityName );
	}
}

void SceneFolderManager::OnEntityRenamed( const std::string& oldName, const std::string& newName )
{
	for ( auto& [ _, folder ] : m_mapFolders )
	{
		if ( folder.ContainsByName( oldName ) )
		{
			folder.RemoveByName( oldName );
			folder.AddByName( newName );
			break; // entity can only be in one folder
		}
	}
}

void SceneFolderManager::SaveToJSON( Scion::Filesystem::JSONSerializer& serializer ) const
{
	serializer.StartNewArray( "sceneFolders" );

	for ( const auto& [ _, folder ] : m_mapFolders )
	{
		serializer.StartNewObject()
			.AddKeyValuePair( "id", folder.id )
			.AddKeyValuePair( "parentId", folder.parentId )
			.AddKeyValuePair( "name", folder.name )
			.AddKeyValuePair( "bExpanded", folder.bExpanded );

		// Entity membership - Stored by Identification::name, not entt::entity handle
		serializer.StartNewArray( "entities" );
		for ( const auto& entityName : folder.entityNames )
			serializer.AddValue( entityName );

		serializer.EndArray();	// Entities
		serializer.EndObject(); // Folder object
	}

	serializer.EndArray();
}

void SceneFolderManager::LoadFromJSON( const rapidjson::Value& foldersArray )
{
	m_mapFolders.clear();
	m_NextId = 1;

	for ( const auto& obj : foldersArray.GetArray() )
	{
		SceneFolder folder{};
		folder.id = obj[ "id" ].GetUint();
		folder.parentId = obj[ "parentId" ].GetUint();
		folder.name = obj[ "name" ].GetString();
		folder.bExpanded = obj.HasMember( "bExpanded" ) && obj[ "bExpanded" ].GetBool();

		// Associate all entities now
		if ( obj.HasMember( "entities" ) )
		{
			for ( const auto& nameVal : obj[ "entities" ].GetArray() )
			{
				folder.entityNames.insert( nameVal.GetString() );
			}
		}

		m_NextId = std::max( m_NextId, folder.id + 1 );
		m_mapFolders.emplace( folder.id, std::move( folder ) );
	}
}

bool SceneFolderManager::FolderExists( const std::string& folderName ) const
{
	auto it = std::ranges::find_if(
		m_mapFolders, [ &folderName ]( const auto& folder ) { return folder.second.name == folderName; } );
	return it != m_mapFolders.end();
}

bool SceneFolderManager::IsAncestor( SceneFolder::Id potentialAncestor, SceneFolder::Id ofId ) const
{
	// Walk up the parent chain from ofId - If we hit a potentialAncestor, it's an ancestor
	SceneFolder::Id current = ofId;
	while ( current != SceneFolder::NoParent )
	{
		const auto* pFolder = GetFolder( current );
		if ( !pFolder )
			break;
		if ( pFolder->parentId == potentialAncestor )
			return true;
		current = pFolder->parentId;
	}

	return false;
}

} // namespace Scion::Editor
