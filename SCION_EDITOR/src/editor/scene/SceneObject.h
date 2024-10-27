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

  public:
	SceneObject( const std::string& sceneName );
	~SceneObject() = default;

	void CopySceneToRuntime();
	void ClearRuntimeScene();

	void AddNewLayer();
	inline std::vector<SCION_UTIL::SpriteLayerParams>& GetLayerParams() { return m_LayerParams; }

	inline Canvas& GetCanvas() { return m_Canvas; }
	inline const std::string& GetName() { return m_sSceneName; }
	inline SCION_CORE::ECS::Registry& GetRegistry() { return m_Registry; }
	inline SCION_CORE::ECS::Registry* GetRegistryPtr() { return &m_Registry; }
	inline SCION_CORE::ECS::Registry& GetRuntimeRegistry() { return m_RuntimeRegistry; }
};
} // namespace SCION_EDITOR
