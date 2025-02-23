#pragma once
#include "Core/ECS/Entity.h"
#include <glm/glm.hpp>

namespace SCION_CORE
{

class Prefab;
class Character;
class Scene;

class PlayerStart
{
  public:
	PlayerStart( SCION_CORE::ECS::Registry& registry, Scene& sceneRef );

	void CreatePlayer( SCION_CORE::ECS::Registry& registry );
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

	inline bool IsCharacterSet() const { return m_pCharacterPrefab != nullptr; }

  private:
	Scene& m_SceneRef;
	SCION_CORE::ECS::Entity m_VisualEntity;
	std::shared_ptr<Prefab> m_pCharacterPrefab;
	std::string m_sCharacterName;
	bool m_bCharacterLoaded;

	// TODO: Determine if this is needed.
	std::shared_ptr<Character> m_pCharacter;
};
} // namespace SCION_CORE
