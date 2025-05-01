#pragma once
#include <map>
#include <unordered_map>
#include <filesystem>
#include <memory>
#include <string>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <thread>

#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>
#include <Rendering/Essentials/Font.h>
#include <Sounds/Essentials/Music.h>
#include <Sounds/Essentials/SoundFX.h>

#include "Core/ECS/Registry.h"
#include <sol/sol.hpp>

namespace SCION_UTIL
{
enum class AssetType;
}

namespace SCION_CORE
{
class Prefab;
}

namespace SCION_RESOURCES
{

class AssetManager
{
  public:
	AssetManager( bool bEnableFilewatcher = false );
	~AssetManager();

	bool CreateDefaultFonts();

	/*
	 * @brief Checks to see if the texture exists, and if not, creates and loads the texture into the
	 * asset manager.
	 * @param An std::string for the texture name to be use as the key.
	 * @param An std::string for the texture file path to be loaded.
	 * @param A bool value to determine if it is pixel art. That controls the type of Min/Mag filter to
	 * use.
	 * @param A bool value to determine if the texture is being used as a tileset.
	 * @return Returns true if the texture was created and loaded successfully, false otherwise.
	 */
	bool AddTexture( const std::string& textureName, const std::string& texturePath, bool pixelArt = true,
					 bool bTileset = false );

	/*
	 * @brief Checks to see if the texture exists, and if not, creates and loads the texture into the asset manager.
	 * @param std::string for the texture name to be used as the key.
	 * @param const unsigned char* this is the image data needed to be converted to an OpenGL texture.
	 * @param size_t length, this is the size of the image data array passed in.
	 * @param A bool value to determine if it is pixel art. That controls the type of Min/Mag filter to use.
	 * @param A bool value to determine if the texture is being used as a tileset.
	 * @return Returns true if the texture was created and loaded successfully, false otherwise.
	 */
	bool AddTextureFromMemory( const std::string& textureName, const unsigned char* imageData, size_t length,
							   bool pixelArt = true, bool bTileset = false );
	/*
	 * @brief Checks to see if the texture exists based on the name and returns a std::shared_ptr<Texture>.
	 * @param An std::string for the texture name to lookup.
	 * @return Returns the desired texture if it exists, else returns nullptr
	 */
	std::shared_ptr<SCION_RENDERING::Texture> GetTexture( const std::string& textureName );

	/*
	 * @brief Get the names of all the textures that are flagged as tilesets.
	 * @return Returns a vector of strings.
	 */
	std::vector<std::string> GetTilesetNames() const;

	/*
	 * @brief Checks to see if the font exists, and if not, creates and loads the font into the
	 * asset manager.
	 * @param An std::string for the font name to be use as the key.
	 * @param An std::string for the texture file path to be loaded.
	 * @param A float for the font size
	 * @return Returns true if the font was created and loaded successfully, false otherwise.
	 */
	bool AddFont( const std::string& fontName, const std::string& fontPath, float fontSize = 32.f );

	/*
	 * @brief Checks to see if the font exists, and if not, creates and loads the font into the
	 * asset manager.
	 * @param An std::string for the font name to be use as the key.
	 * @param An unsigned char* of the font data
	 * @param A float for the font size
	 * @return Returns true if the font was created and loaded successfully, false otherwise.
	 */
	bool AddFontFromMemory( const std::string& fontName, unsigned char* fontData, float fontSize = 32.f );

	/*
	 * @brief Checks to see if the font exists based on the name and returns a std::shared_ptr<Font>.
	 * @param An std::string for the font name to lookup.
	 * @return Returns the desired font if it exists, else returns nullptr
	 */
	std::shared_ptr<SCION_RENDERING::Font> GetFont( const std::string& fontName );

	/*
	 * @brief Checks to see if the Shader exists, and if not, creates and loads the Shader into the
	 * asset manager.
	 * @param An std::string for the shader name to be use as the key.
	 * @param An std::string for the vertex shader file path to be loaded.
	 * @param An std::string for the fragment shader file path to be loaded.
	 * @return Returns true if the shader was created and loaded successfully, false otherwise.
	 */
	bool AddShader( const std::string& shaderName, const std::string& vertexPath, const std::string& fragmentPath );

	/*
	 * @brief Checks to see if the Shader exists, and if not, creates and loads the Shader into the
	 * asset manager.
	 * @param An std::string for the shader name to be use as the key.
	 * @param A const char* for the vertex shader memory to be loaded.
	 * @param A const char* for the fragment shader memory to be loaded.
	 * @return Returns true if the shader was created and loaded successfully, false otherwise.
	 */
	bool AddShaderFromMemory( const std::string& shaderName, const char* vertexShader, const char* fragmentShader );

	/*
	 * @brief Checks to see if the shader exists based on the name and returns shared_ptr<Shader>.
	 * @param An std::string for the shader name to lookup.
	 * @return Returns the desired shader if it exists, else returns nullptr
	 */
	std::shared_ptr<SCION_RENDERING::Shader> GetShader( const std::string& shaderName );

	/*
	 * @brief Checks to see if the Music exists, and if not, creates and loads the Music into the
	 * asset manager.
	 * @param An std::string for the music name to be use as the key.
	 * @param An std::string for the filepath where the music file is located.
	 * @return Returns true if the music was created and loaded successfully, false otherwise.
	 */
	bool AddMusic( const std::string& musicName, const std::string& filepath );

	/*
	 * @brief Checks to see if the music exists based on the name and returns shared_ptr<Music>.
	 * @param An std::string for the music name to lookup.
	 * @return Returns an std::shared_ptr<Music> if it exists, else returns nullptr
	 */
	std::shared_ptr<SCION_SOUNDS::Music> GetMusic( const std::string& musicName );

	/*
	 * @brief Checks to see if the SoundFx exists, and if not, creates and loads the SoundFx into the
	 * asset manager.
	 * @param An std::string for the SoundFx name to be use as the key.
	 * @param An std::string for the filepath where the SoundFx file is located.
	 * @return Returns true if the Soundfx was created and loaded successfully, false otherwise.
	 */
	bool AddSoundFx( const std::string& soundFxName, const std::string& filepath );

	/*
	 * @brief Checks to see if the soundFx exists based on the name and returns shared_ptr<SoundFX>.
	 * @param An std::string for the SoundFx name to lookup.
	 * @return Returns an std::shared_ptr<SoundFx> if it exists, else returns nullptr
	 */
	std::shared_ptr<SCION_SOUNDS::SoundFX> GetSoundFx( const std::string& soundFxName );

	bool AddPrefab( const std::string& sPrefabName, std::shared_ptr<SCION_CORE::Prefab> pPrefab );

	std::shared_ptr<SCION_CORE::Prefab> GetPrefab( const std::string& sPrefabName );

	inline const std::map<std::string, std::shared_ptr<SCION_RENDERING::Texture>>& GetAllTextures() const
	{
		return m_mapTextures;
	}

	inline const std::map<std::string, std::shared_ptr<SCION_SOUNDS::SoundFX>>& GetAllSoundFx() const
	{
		return m_mapSoundFx;
	}

	inline const std::map<std::string, std::shared_ptr<SCION_RENDERING::Shader>>& GetAllShaders() const
	{
		return m_mapShader;
	}

	inline const std::map<std::string, std::shared_ptr<SCION_RENDERING::Font>>& GetAllFonts() const
	{
		return m_mapFonts;
	}

	inline const std::map<std::string, std::shared_ptr<SCION_SOUNDS::Music>>& GetAllMusic() const { return m_mapMusic; }

	inline const std::map<std::string, std::shared_ptr<SCION_CORE::Prefab>>& GetAllPrefabs() const
	{
		return m_mapPrefabs;
	}

	/*
	 * @brief Grab all the asset key names for the specific asset type.
	 * @param Takes in an enum for the desired AssetType.
	 * @return Returns an std::vector of strings of the asset names.
	 */
	std::vector<std::string> GetAssetKeyNames( SCION_UTIL::AssetType eAssetType ) const;

	/*
	 * @brief Try to change the name of the asset based on the asset type.
	 * @param std::string of the old asset name.
	 * @param std::string of the new asset name.
	 * @param An enum of the asset type.
	 * @return Returns true if the name was changed successfully, false otherwise.
	 */
	bool ChangeAssetName( const std::string& sOldName, const std::string& sNewName, SCION_UTIL::AssetType eAssetType );

	/*
	 * @brief Checks to see if the asset exists.
	 * @param std::string of the name to check.
	 * @param an enum of the asset type to check.
	 * @return Returns true if the asset exists, false otherwise.
	 */
	bool CheckHasAsset( const std::string& sNameCheck, SCION_UTIL::AssetType eAssetType );

	/*
	 * @brief Checks to see if the asset exists based on it's name and type. Then deletes the asset.
	 * @param std::string of the name to check.
	 * @param an enum of the asset type to check.
	 * @return Returns true if the asset was deleted successfully, false otherwise.
	 */
	bool DeleteAsset( const std::string& sAssetName, SCION_UTIL::AssetType eAssetType );

	/*
	 * Binds the AssetManager functionality to the lua state.
	 * @param takes in the sol::state& for binding to lua.
	 * @param also takes in the registry. The Asset Manager for lua should already exist in the
	 * main registry.
	 */
	static void CreateLuaAssetManager( sol::state& lua );

	void Update();

  private:
	void FileWatcher();

	struct AssetWatchParams
	{
		std::string sAssetName{ "" };
		std::string sFilepath{ "" };
		std::filesystem::file_time_type lastWrite;
		SCION_UTIL::AssetType eType{};
		bool bDirty{ false };
	};

	void ReloadAsset( const AssetWatchParams& assetParams );

	void ReloadTexture( const std::string& sTextureName );
	void ReloadSoundFx( const std::string& sSoundName );
	void ReloadMusic( const std::string& sMusicName );
	void ReloadFont( const std::string& sFontName );
	void ReloadShader( const std::string& sShaderName );

  private:


	std::map<std::string, std::shared_ptr<SCION_RENDERING::Texture>> m_mapTextures{};
	std::map<std::string, std::shared_ptr<SCION_RENDERING::Shader>> m_mapShader{};
	std::map<std::string, std::shared_ptr<SCION_RENDERING::Font>> m_mapFonts{};

	std::map<std::string, std::shared_ptr<SCION_SOUNDS::Music>> m_mapMusic{};
	std::map<std::string, std::shared_ptr<SCION_SOUNDS::SoundFX>> m_mapSoundFx{};
	std::map<std::string, std::shared_ptr<SCION_CORE::Prefab>> m_mapPrefabs{};

	std::vector<AssetWatchParams> m_FilewatchParams;

	std::atomic<bool> m_bFileWatcherRunning;
	std::jthread m_WatchThread;
	std::mutex m_CallbackMutex;
	std::shared_mutex m_AssetMutex;
};
} // namespace SCION_RESOURCES
