#pragma once
#include "Core/ECS/Entity.h"
#include "editor/utilities/EditorUtilities.h"
#include "ScionUtilities/HelperUtilities.h"

namespace SCION_EDITOR
{
class SceneObject
{
  private:
	SCION_CORE::ECS::Registry m_Registry, m_RuntimeRegistry;
	std::string m_sSceneName;
	Canvas m_Canvas;
	std::vector<SCION_UTIL::SpriteLayerParams> m_LayerParams;
	int m_CurrentLayer;

  public:
	SceneObject( const std::string& sceneName );
	~SceneObject() = default;

	void CopySceneToRuntime();
	void ClearRuntimeScene();

	// Scene Tilemap Layer functions
	/*
	* @brief Adds a new layer to the scene tilemap layers.
	*/
	void AddNewLayer();

	/*
	* @brief Checks to see if the layer already exists in the scene.
	* To be used when trying to adjust the name of the layer to prevent layers with duplicate names.
	* @param Takes in a string for the layer to check.
	* @return Returns true if that layer does not exist, false otherwise.
	*/
	bool CheckLayerName( const std::string& sLayerName );

	/*
	* @brief Checks to see if there are any layers in the scenes tilemap.
	* @return Returns true if there are layers present, false otherwise.
	*/
	bool HasTileLayers() const { return !m_LayerParams.empty(); }
	inline std::vector<SCION_UTIL::SpriteLayerParams>& GetLayerParams() { return m_LayerParams; }

	inline Canvas& GetCanvas() { return m_Canvas; }
	inline const std::string& GetName() { return m_sSceneName; }
	inline SCION_CORE::ECS::Registry& GetRegistry() { return m_Registry; }
	inline SCION_CORE::ECS::Registry* GetRegistryPtr() { return &m_Registry; }
	inline SCION_CORE::ECS::Registry& GetRuntimeRegistry() { return m_RuntimeRegistry; }
};
} // namespace SCION_EDITOR
