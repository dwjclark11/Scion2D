#pragma once
#include "Core/ECS/Components/AllComponents.h"

namespace Scion::Core
{

namespace ECS
{
class Registry;
class Entity;
} // namespace ECS

enum class EPrefabType
{
	Character,
	GameObject,
	/* TODO: Add more Prefab types */
	InvalidType
};

struct PrefabbedEntity;

/* Prefabbed Relationships basically need to be able to build all of the necessary entities */
struct PrefabbedRelationships
{
	/* The first child of the entity. */
	PrefabbedEntity* firstChild{ nullptr };
	/* The previous sibling entity in the hierarchy for the parent. */
	PrefabbedEntity* prevSibling{ nullptr };
	/* The next sibling entity in the hierarchy for the parent. */
	PrefabbedEntity* nextSibling{ nullptr };
	/* The parent entity. Not all entities will have parents. */
	PrefabbedEntity* parent{ nullptr };
};

struct PrefabbedEntity
{
	Scion::Core::ECS::TransformComponent transform{};
	std::optional<Scion::Core::ECS::AnimationComponent> animation{ std::nullopt };
	std::optional<Scion::Core::ECS::SpriteComponent> sprite{ std::nullopt };
	std::optional<Scion::Core::ECS::BoxColliderComponent> boxCollider{ std::nullopt };
	std::optional<Scion::Core::ECS::CircleColliderComponent> circleCollider{ std::nullopt };
	std::optional<Scion::Core::ECS::PhysicsComponent> physics{ std::nullopt };
	std::optional<Scion::Core::ECS::RigidBodyComponent> rigidBody{ std::nullopt };
	std::optional<Scion::Core::ECS::Identification> id{ std::nullopt };
	std::optional<Scion::Core::ECS::TextComponent> textComp{ std::nullopt };
	std::optional<Scion::Core::ECS::UIComponent> uiComp{ std::nullopt };
	std::optional<PrefabbedRelationships> relationships{ std::nullopt };
};

/* Prefabs cannot be associated with any registry. The need to be able to create entities when dragged into a level. */
class Prefab
{
  public:
	Prefab();
	Prefab( EPrefabType eType, const PrefabbedEntity& prefabbed );
	Prefab( const std::string& sPrefabPath );
	~Prefab();

	bool Load( const std::string& sPrefabPath );
	bool Save();

	inline const PrefabbedEntity& GetPrefabbedEntity() const { return m_Entity; }
	inline const std::string& GetFilepath() const { return m_sPrefabPath; }
	inline EPrefabType GetType() const { return m_eType; }

  private:
	void AddChild( const PrefabbedEntity& child );

  private:
	EPrefabType m_eType;
	PrefabbedEntity m_Entity;
	std::vector<std::shared_ptr<PrefabbedEntity>> m_RelatedPrefabs;
	std::string m_sName;
	std::string m_sPrefabPath;

	friend class PrefabCreator;
};

class PrefabCreator
{
  public:
	PrefabCreator() = delete;
	static std::shared_ptr<Prefab> CreatePrefab( EPrefabType eType, Scion::Core::ECS::Entity& entityToPrefab );
	static std::shared_ptr<Prefab> CreatePrefab( const std::string& sPrefabPath );
	static std::shared_ptr<Scion::Core::ECS::Entity> AddPrefabToScene( const Prefab& prefab,
																	  Scion::Core::ECS::Registry& registry );
	static bool DeletePrefab( Prefab& prefabToDelete );
};

} // namespace Scion::Core
