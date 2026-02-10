#pragma once
#include "Core/ECS/Entity.h"
#include <glm/glm.hpp>

namespace Scion::Core
{

class Prefab;
class Character;
class Scene;

class PlayerStart
{
  public:
	PlayerStart( Scion::Core::ECS::Registry& registry, Scene& sceneRef );

	void CreatePlayer( Scion::Core::ECS::Registry& registry );
	std::string GetCharacterName();
	void SetCharacter( const Prefab& prefab );

	glm::vec2 GetPosition();
	void SetPosition( const glm::vec2& position );

	/*
	 * @brief Loads the character name and sets the prefab to be loaded.
	 * The prefabs might not be loaded when loading the scenes that owns this
	 * player start.
	 */
	void Load( const std::string& sPrefabName );
	void Unload();
	void LoadVisualEntity();

	inline bool IsCharacterSet() const { return m_pCharacterPrefab != nullptr; }
	inline bool IsPlayerStartCreated() const { return m_bVisualEntityCreated; }
	
  private:
	Scene& m_SceneRef;
	std::shared_ptr<Scion::Core::ECS::Entity> m_pVisualEntity;
	std::shared_ptr<Prefab> m_pCharacterPrefab;
	std::string m_sCharacterName;
	bool m_bCharacterLoaded;
	bool m_bVisualEntityCreated;

	// TODO: Determine if this is needed.
	std::shared_ptr<Character> m_pCharacter;
};
} // namespace Scion::Core
