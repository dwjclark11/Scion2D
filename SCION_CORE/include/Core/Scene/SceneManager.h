#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace SCION_CORE
{

enum class EMapType;
class Scene;

class SceneManager
{
  public:
	SceneManager();
	virtual ~SceneManager() {}

	virtual bool AddScene( const std::string& sSceneName, SCION_CORE::EMapType eType );
	bool HasScene( const std::string& sSceneName );

	Scene* GetScene( const std::string& sSceneName );
	Scene* GetCurrentScene();

	std::vector<std::string> GetSceneNames() const;
	bool LoadCurrentScene();
	bool UnloadCurrentScene();
	bool CheckHasScene( const std::string& sSceneName );
	bool ChangeSceneName( const std::string& sOldName, const std::string& sNewName );

	inline void SetCurrentScene( const std::string& sSceneName ) { m_sCurrentScene = sSceneName; }
	inline const std::string& GetCurrentSceneName() const { return m_sCurrentScene; }

  protected:
	std::map<std::string, std::shared_ptr<Scene>> m_mapScenes;
	std::string m_sCurrentScene{ "" };
	std::string m_sCurrentTileset{ "" };
};
} // namespace SCION_CORE
