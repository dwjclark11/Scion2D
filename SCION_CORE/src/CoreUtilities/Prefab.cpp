#include "Core/CoreUtilities/Prefab.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/ComponentSerializer.h"
#include "Core/CoreUtilities/SaveProject.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/Scene/Scene.h"

#include "Core/Resources/AssetManager.h"

#include "ScionFilesystem/Serializers/JSONSerializer.h"
#include "ScionUtilities/ScionUtilities.h"
#include "ScionUtilities/HelperUtilities.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <fmt/format.h>

#include <filesystem>

namespace fs = std::filesystem;

#include "Logger/Logger.h"

using namespace SCION_FILESYSTEM;
using namespace SCION_CORE::ECS;

namespace SCION_CORE
{

Prefab::Prefab()
	: m_eType{ EPrefabType::GameObject }
	, m_Entity{}
	, m_RelatedPrefabs{}
	, m_sName{}
	, m_sPrefabPath{}
{
	// EMPTY PREFAB
}

Prefab::Prefab( EPrefabType eType, const PrefabbedEntity& prefabbed )
	: m_eType{ eType }
	, m_Entity{ prefabbed }
	, m_RelatedPrefabs{}
	, m_sName{}
	, m_sPrefabPath{}
{
	SCION_ASSERT( prefabbed.id && "Must have an ID Component" );
	SCION_ASSERT( !prefabbed.id->name.empty() && "All prefabs must have unique names!" );

	m_sName = prefabbed.id->name + "_pfab";
	m_Entity.id->name = m_sName;

	auto& pSaveProject = MAIN_REGISTRY().GetContext<std::shared_ptr<SCION_CORE::SaveProject>>();
	SCION_ASSERT( pSaveProject && "SaveProject must exists here!" );

	m_sPrefabPath = fmt::format( "{}content{}assets{}prefabs{}{}",
								 pSaveProject->sProjectPath,
								 PATH_SEPARATOR,
								 PATH_SEPARATOR,
								 PATH_SEPARATOR,
								 m_sName + ".json" );

	if ( fs::exists( m_sPrefabPath ) )
	{
		SCION_ERROR( "Failed to create prefab. [{}] Already exists!", m_sName );
		throw std::runtime_error( fmt::format( "Failed to create prefab. [{}] Already exists!", m_sName ).c_str() );
	}

	Save();
}

Prefab::Prefab( const std::string& sPrefabPath )
	: m_eType{ EPrefabType::GameObject }
	, m_Entity{}
	, m_RelatedPrefabs{}
	, m_sName{}
	, m_sPrefabPath{ sPrefabPath }
{
	if ( !Load( sPrefabPath ) )
	{
		SCION_ERROR( "Failed to load prefab from path [{}]", sPrefabPath );
		throw std::runtime_error( fmt::format( "Failed to load prefab from path [{}]", sPrefabPath ).c_str() );
	}
}

Prefab::~Prefab()
{
}

void Prefab::AddChild( const PrefabbedEntity& child )
{
	auto& prefabbedChild = m_RelatedPrefabs.emplace_back( std::make_shared<PrefabbedEntity>( child ) );
	if ( !m_Entity.relationships )
	{
		m_Entity.relationships = PrefabbedRelationships{};
		m_Entity.relationships->firstChild = prefabbedChild.get();
		return;
	}

	auto& relations = m_Entity.relationships.value();
	if ( !prefabbedChild->relationships )
	{
		prefabbedChild->relationships = PrefabbedRelationships{};
	}
	auto& childRelations = prefabbedChild->relationships.value();
	if ( auto* prevSibling = childRelations.prevSibling )
	{
		if ( prevSibling->relationships )
		{
			prevSibling->relationships->nextSibling = childRelations.nextSibling;
		}
	}

	if ( auto* nextSibling = childRelations.nextSibling )
	{
		if ( nextSibling->relationships )
		{
			nextSibling->relationships->prevSibling = childRelations.prevSibling;
		}
	}

	if ( auto* parent = childRelations.parent )
	{
		if ( parent->relationships )
		{
			if ( parent->relationships->firstChild == prefabbedChild.get() )
			{
				parent->relationships->firstChild = nullptr;
				if ( auto* nextSibling = childRelations.nextSibling )
				{
					if ( nextSibling->relationships )
					{
						parent->relationships->firstChild = nextSibling;
						nextSibling->relationships->prevSibling = nullptr;
					}
				}
			}
		}
	}

	// Reset the child's siblings
	childRelations.nextSibling = nullptr;
	childRelations.prevSibling = nullptr;

	childRelations.parent = &m_Entity;

	if ( !relations.firstChild )
	{
		relations.firstChild = prefabbedChild.get();
	}
	else
	{
		// TODO: Set sibling links
	}
}

bool Prefab::Load( const std::string& sPrefabPath )
{
	std::ifstream prefabFile;
	prefabFile.open( sPrefabPath );

	if ( !prefabFile.is_open() )
	{
		SCION_ERROR( "Failed to open tilemap file [{}]", sPrefabPath );
		return false;
	}

	// If the prefab file is empty, it is not valid
	if ( prefabFile.peek() == std::ifstream::traits_type::eof() )
	{
		SCION_ERROR( "Failed to load prefab file [{}] - Empty prefab files are invalid.", sPrefabPath );
		return false;
	}

	std::stringstream ss;
	ss << prefabFile.rdbuf();
	std::string contents = ss.str();
	rapidjson::StringStream jsonStr{ contents.c_str() };

	rapidjson::Document doc;
	doc.ParseStream( jsonStr );

	if ( doc.HasParseError() || !doc.IsObject() )
	{
		SCION_ERROR( "Failed to load prefab: File: [{}] is not valid JSON. - {} - {}",
					 sPrefabPath,
					 rapidjson::GetParseError_En( doc.GetParseError() ),
					 doc.GetErrorOffset() );
		return false;
	}

	const rapidjson::Value& prefab = doc[ "prefab" ];
	if ( !prefab.HasMember( "components" ) )
	{
		SCION_ERROR( "Failed to load prefab: File: [{}] - Prefabs must have components.", sPrefabPath );
		return false;
	}

	const rapidjson::Value& components = prefab[ "components" ];

	if ( components.HasMember( "id" ) )
	{
		const rapidjson::Value& id = components[ "id" ];
		Identification idComp{};
		DESERIALIZE_COMPONENT( id, idComp );
		m_Entity.id = idComp;

		m_sName = m_Entity.id->name;
	}

	if ( components.HasMember( "transform" ) )
	{
		// The transform position does not matter.
		const rapidjson::Value& transform = components[ "transform" ];
		m_Entity.transform.scale =
			glm::vec2{ transform[ "scale" ][ "x" ].GetFloat(), transform[ "scale" ][ "y" ].GetFloat() };
		m_Entity.transform.rotation = transform[ "rotation" ].GetFloat();
	}

	if ( components.HasMember( "sprite" ) )
	{
		const rapidjson::Value& sprite = components[ "sprite" ];
		SpriteComponent spriteComp{};
		DESERIALIZE_COMPONENT( sprite, spriteComp );
		m_Entity.sprite = spriteComp;
	}

	if ( components.HasMember( "animation" ) )
	{
		const rapidjson::Value& animation = components[ "animation" ];
		AnimationComponent animationComp{};
		DESERIALIZE_COMPONENT( animation, animationComp );
		m_Entity.animation = animationComp;
	}

	if ( components.HasMember( "boxCollider" ) )
	{
		const rapidjson::Value& boxCollider = components[ "boxCollider" ];
		BoxColliderComponent boxColliderComp{};
		DESERIALIZE_COMPONENT( boxCollider, boxColliderComp );
		m_Entity.boxCollider = boxColliderComp;
	}

	if ( components.HasMember( "circleCollider" ) )
	{
		const rapidjson::Value& circleCollider = components[ "circleCollider" ];
		CircleColliderComponent circleColliderComp{};
		DESERIALIZE_COMPONENT( circleCollider, circleColliderComp );
		m_Entity.circleCollider = circleColliderComp;
	}

	if ( components.HasMember( "physics" ) )
	{
		const rapidjson::Value& physics = components[ "physics" ];
		PhysicsComponent physicsComp{};
		DESERIALIZE_COMPONENT( physics, physicsComp );
		m_Entity.physics = physicsComp;
	}

	if ( components.HasMember( "text" ) )
	{
		const rapidjson::Value& text = components[ "text" ];
		TextComponent textComp{};
		DESERIALIZE_COMPONENT( text, textComp );
		m_Entity.textComp = textComp;
	}

	// TODO: All other components
	prefabFile.close();

	return true;
}

bool Prefab::Save()
{
	std::unique_ptr<JSONSerializer> pSerializer{ nullptr };

	try
	{
		pSerializer = std::make_unique<JSONSerializer>( m_sPrefabPath );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to save game objects[{}] - [{}]", m_sPrefabPath, ex.what() );
		return false;
	}

	fs::path prefabPath{ m_sPrefabPath };
	if ( !fs::exists( prefabPath ) )
	{
		SCION_ERROR( "Failed to save game objects - Filepath does not exist [{}]", m_sPrefabPath );
		return false;
	}

	pSerializer->StartDocument();
	pSerializer->StartNewObject( "prefab" );

	pSerializer->StartNewObject( "components" );

	SERIALIZE_COMPONENT( *pSerializer, m_Entity.transform );
	const auto& id = m_Entity.id.value();
	SERIALIZE_COMPONENT( *pSerializer, id );

	if ( m_Entity.sprite )
	{
		const auto& sprite = m_Entity.sprite.value();
		SERIALIZE_COMPONENT( *pSerializer, sprite );
	}

	if ( m_Entity.animation )
	{
		const auto& animation = m_Entity.animation.value();
		SERIALIZE_COMPONENT( *pSerializer, animation );
	}

	if ( m_Entity.boxCollider )
	{
		const auto& boxCollider = m_Entity.boxCollider.value();
		SERIALIZE_COMPONENT( *pSerializer, boxCollider );
	}

	if ( m_Entity.circleCollider )
	{
		const auto& circleCollider = m_Entity.circleCollider.value();
		SERIALIZE_COMPONENT( *pSerializer, circleCollider );
	}

	if ( m_Entity.physics )
	{
		const auto& physics = m_Entity.physics.value();
		SERIALIZE_COMPONENT( *pSerializer, physics );
	}

	if ( m_Entity.rigidBody )
	{
		const auto& rigidBody = m_Entity.rigidBody.value();
		SERIALIZE_COMPONENT( *pSerializer, rigidBody );
	}

	if ( m_Entity.textComp )
	{
		const auto& textComp = m_Entity.textComp.value();
		SERIALIZE_COMPONENT( *pSerializer, textComp );
	}

	// TODO: Create serialize UI component
	/*if (m_Entity.uiComp)
	{
		const auto& ui = m_Entity.uiComp.value();
		SERIALIZE_COMPONENT( *pSerializer, ui );
	}*/

	pSerializer->EndObject(); // End Components
	pSerializer->EndObject(); // End Prefab
	pSerializer->EndDocument();
	return true;
}

std::shared_ptr<Prefab> PrefabCreator::CreatePrefab( EPrefabType eType, SCION_CORE::ECS::Entity& entityToPrefab )
{
	PrefabbedEntity prefabbed{};

	if ( auto* transform = entityToPrefab.TryGetComponent<TransformComponent>() )
	{
		prefabbed.transform = *transform;
		prefabbed.transform.position = glm::vec2{ 0.f };
	}

	if ( auto* sprite = entityToPrefab.TryGetComponent<SpriteComponent>() )
	{
		prefabbed.sprite = *sprite;
		auto pTexture = ASSET_MANAGER().GetTexture( sprite->sTextureName );
		SCION_ASSERT( pTexture && "Sprite texture must exist in the asset manager." );
		GenerateUVs( *prefabbed.sprite, pTexture->GetWidth(), pTexture->GetHeight() );
	}

	if ( auto* boxCollider = entityToPrefab.TryGetComponent<BoxColliderComponent>() )
	{
		prefabbed.boxCollider = *boxCollider;
	}

	if ( auto* circleCollider = entityToPrefab.TryGetComponent<CircleColliderComponent>() )
	{
		prefabbed.circleCollider = *circleCollider;
	}

	if ( auto* physics = entityToPrefab.TryGetComponent<PhysicsComponent>() )
	{
		prefabbed.physics = *physics;
	}

	if ( auto* animation = entityToPrefab.TryGetComponent<AnimationComponent>() )
	{
		prefabbed.animation = *animation;
	}

	if ( auto* rigidBody = entityToPrefab.TryGetComponent<RigidBodyComponent>() )
	{
		prefabbed.rigidBody = *rigidBody;
	}

	if ( auto* id = entityToPrefab.TryGetComponent<Identification>() )
	{
		prefabbed.id = *id;
	}

	if ( auto* ui = entityToPrefab.TryGetComponent<UIComponent>() )
	{
		prefabbed.uiComp = *ui;
	}

	if ( auto* text = entityToPrefab.TryGetComponent<TextComponent>() )
	{
		prefabbed.textComp = *text;
	}

	if ( auto* relations = entityToPrefab.TryGetComponent<Relationship>() )
	{
		// TODO: handle relationships
	}

	try
	{
		return std::make_shared<Prefab>( eType, prefabbed );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to create prefab. {}", ex.what() );
	}

	return nullptr;
}

std::shared_ptr<Prefab> PrefabCreator::CreatePrefab( const std::string& sPrefabPath )
{
	try
	{
		return std::make_shared<Prefab>( sPrefabPath );
	}
	catch ( const std::exception& ex )
	{
		SCION_ERROR( "Failed to create prefab. {}", ex.what() );
	}

	return nullptr;
}

std::shared_ptr<SCION_CORE::ECS::Entity> PrefabCreator::AddPrefabToScene( const Prefab& prefab,
																		  SCION_CORE::ECS::Registry& registry )
{
	const auto& prefabbed = prefab.GetPrefabbedEntity();
	auto newEnt = std::make_shared<SCION_CORE::ECS::Entity>( registry, prefabbed.id->name, prefabbed.id->group );

	newEnt->AddComponent<TransformComponent>( prefabbed.transform );
	if ( prefabbed.sprite )
	{
		newEnt->AddComponent<SpriteComponent>( prefabbed.sprite.value() );
	}

	if ( prefabbed.animation )
	{
		newEnt->AddComponent<AnimationComponent>( prefabbed.animation.value() );
	}

	if ( prefabbed.boxCollider )
	{
		newEnt->AddComponent<BoxColliderComponent>( prefabbed.boxCollider.value() );
	}

	if ( prefabbed.circleCollider )
	{
		newEnt->AddComponent<CircleColliderComponent>( prefabbed.circleCollider.value() );
	}

	if ( prefabbed.textComp )
	{
		newEnt->AddComponent<TextComponent>( prefabbed.textComp.value() );
	}

	if ( prefabbed.physics )
	{
		newEnt->AddComponent<PhysicsComponent>( prefabbed.physics.value() );
	}

	return newEnt;
}

bool PrefabCreator::DeletePrefab( Prefab& prefabToDelete )
{
	fs::path prefabPath{ prefabToDelete.GetFilepath() };

	std::error_code ec;
	if ( !fs::exists( prefabPath, ec ) )
	{
		SCION_ERROR( "Failed to delete prefab. {}", ec.message() );
		return false;
	}

	if ( fs::is_directory( prefabPath ) || !prefabPath.has_extension() )
	{
		SCION_ERROR( "Failed to delete prefab. The path is a directory. To delete, we must pass in the file." );
		return false;
	}

	if ( !fs::remove( prefabPath, ec ) )
	{
		SCION_ERROR( "Failed to delete prefab. {}", ec.message() );
		return false;
	}

	// Recheck to ensure file was deleted.
	if ( fs::exists( prefabPath ) )
	{
		SCION_ERROR( "Failed to delete prefab. File not deleted correctly." );
		return false;
	}

	return true;
}

} // namespace SCION_CORE
