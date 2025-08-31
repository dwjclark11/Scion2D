#pragma once
#include <glm/glm.hpp>
#include <sol/sol.hpp>
#include "Core/ECS/Entity.h"
#include "Core/Character/PlayerStart.h"
#include "ScionUtilities/HelperUtilities.h"

namespace SCION_CORE
{
struct Canvas
{
	int width{ 640 };
	int height{ 480 };
	int tileWidth{ 16 };
	int tileHeight{ 16 };
	glm::vec2 offset{ 0.f };
};

enum class EMapType
{
	Grid,
	IsoGrid
};

class Scene
{
  public:
	Scene();
	Scene( const std::string& sceneName, EMapType eType );
	virtual ~Scene() {};

	/*
	 * @brief Tries to load the scene. Loads the tilemap, layers,
	 * game objects, and other scene data.
	 * @return Returns true if successful, false otherwise.
	 */
	virtual bool LoadScene();

	/*
	 * @brief Tries to Unload the scene. When unloading, this will
	 * try to save the tilemap, layers, game objects, and other scene data.
	 * The scene's registry will also be cleared.
	 * @return Returns true if successful, false otherwise.
	 */
	virtual bool UnloadScene( bool bSaveScene = true );

	/*
	 * @brief Tries to add a specific layer and set it's visibility.
	 * If the layer already exists, it will do nothing.
	 * @param std::string for the layer name.
	 * @param bool to set the visibility of the layer.
	 */
	int AddLayer( const std::string& sLayerName, bool bVisible );

	int AddLayer( const SCION_UTIL::SpriteLayerParams& layerParam );

	/*
	 * @brief Checks to see if the layer already exists in the scene.
	 * To be used when trying to adjust the name of the layer to prevent layers with duplicate names.
	 * @param Takes in a string for the layer to check.
	 * @return Returns true if that layer does not exist, false otherwise.
	 */
	bool CheckLayerName( const std::string& sLayerName );

	/*
	 * @brief Tries to save the scene. This differs from the unload function
	 * because it does not set the loaded flag or clear the registry.
	 * @return Returns true if successful, false otherwise.
	 */
	bool SaveScene( bool bOverride = false ) { return SaveSceneData( bOverride ); }

	inline const std::string& GetDefaultMusicName() const { return m_sDefaultMusic; }
	inline void SetDefaultMusic( const std::string& sDefaultMusic ) { m_sDefaultMusic = sDefaultMusic; }

	inline Canvas& GetCanvas() { return m_Canvas; }
	inline SCION_CORE::ECS::Registry& GetRegistry() { return m_Registry; }
	inline SCION_CORE::ECS::Registry* GetRegistryPtr() { return &m_Registry; }
	inline const std::string& GetSceneDataPath() { return m_sSceneDataPath; }
	inline EMapType GetMapType() const { return m_eMapType; }
	inline bool IsLoaded() const { return m_bSceneLoaded; }
	/*
	 * @brief Checks to see if there are any layers in the scenes tilemap.
	 * @return Returns true if there are layers present, false otherwise.
	 */
	inline bool HasTileLayers() const { return !m_LayerParams.empty(); }
	inline std::vector<SCION_UTIL::SpriteLayerParams>& GetLayerParams() { return m_LayerParams; }

	inline PlayerStart& GetPlayerStart() { return m_PlayerStart; }
	inline bool IsPlayerStartEnabled() const { return m_bUsePlayerStart; }
	inline void SetPlayerStartEnabled( bool bEnable ) { m_bUsePlayerStart = bEnable; }
	inline const std::string& GetSceneName() const { return m_sSceneName; }
	inline const std::string& GetFilepath() const { return m_sSceneDataPath; }

	static void CreateLuaBind( sol::state& lua );

  protected:
	bool LoadSceneData();
	bool SaveSceneData(bool bOverride = false);
	void SetCanvasOffset();
	
  protected:
	/* The registry that is used in the tilemap editor and the scene hierarchy. */
	SCION_CORE::ECS::Registry m_Registry;
	/* The name of the scene object represents. */
	std::string m_sSceneName;
	/* The filepath which to load and save the tilemap. */
	std::string m_sTilemapPath;
	/* The filepath which to load and save the game objects. */
	std::string m_sObjectPath;
	/* The filepath which to load and save the scene data. */
	std::string m_sSceneDataPath;
	/* The default music to play for the scene. */
	std::string m_sDefaultMusic;
	/* Has this scene been loaded in the editor. */
	bool m_bSceneLoaded;
	/* All scenes will have a default player start. This flag will control it's use. */
	bool m_bUsePlayerStart;

	SCION_CORE::Canvas m_Canvas;

	/* The type of map to create tiles. Iso or regular grid. */
	EMapType m_eMapType;

	std::vector<SCION_UTIL::SpriteLayerParams> m_LayerParams;

	PlayerStart m_PlayerStart;
};

} // namespace SCION_CORE
