#pragma once
#include <string>
#include <unordered_map>
#include <filesystem>
#include <optional>
#include <memory>

namespace fs = std::filesystem;

namespace SCION_RENDERING
{
class Texture;
}

namespace SCION_CORE
{

// clang-format off
/*
* @brief Enum class defining the different folder types within a project
*/
enum class EProjectFolderType
{
	Content,			// Root content folder (e.g., for user assets)
	Config,				// General configuration files
	GameConfig,			// Game-specific configuration files
	EditorConfig,		// Editor-specific settings and configuration files
	Assets,				// Folder containing all asset types
	Scripts,			// Game lua scripts
	Textures,			// Texture files (e.g. PNG, JPG)
	Shaders,			// User GLSL shader files
	Prefabs,			// Prefab definitions (Serialized Prefabbed entitites)
	Fonts,				// Font files use in the game.
	Scenes,				// Scene or level serialized files
	SoundFx,			// Sound Fx files
	Music,				// Background music files
};
// clang-format on

/*
* @brief ProjectInfo - Class holding all metadata and state related to a single project.
*
* Holds the core project structure, including file paths, metadata, and configuration
* used by the Scion2D game engine editor.
*/
class ProjectInfo
{
  public:
	/** @brief Default constructor. */
	ProjectInfo() = default;
	/** @brief Default destructor. */
	~ProjectInfo() = default;

	/**
	 * @brief Sets the root path of the project.
	 * @param path The root filesystem path.
	 */
	void SetProjectPath( const fs::path& path );

	/**
	 * @brief Gets the root path of the project.
	 * @return A constant reference to the path.
	 */
	const fs::path& GetProjectPath() const;

	/**
	 * @brief Sets the path to the project file (.s2dprj).
	 * @param path Full path to the project file.
	 */
	void SetProjectFilePath( const fs::path& path );

	/**
	 * @brief Gets the path to the project file, if set.
	 * @return Optional containing path or empty if not set.
	 */
	std::optional<fs::path> GetProjectFilePath() const;

	/**
	 * @brief Sets the path to the main Lua script (entry point).
	 * @param path Path to the Lua script.
	 */
	void SetMainLuaScriptPath( const fs::path& path );

	/**
	 * @brief Gets the main Lua script path, if available.
	 * @return Optional containing path or empty if not set.
	 */
	std::optional<fs::path> GetMainLuaScriptPath() const;

	/**
	 * @brief Sets the path to a script list used for scripts to load automatically.
	 * @param path File path to the script list.
	 */
	void SetScriptListPath( const fs::path& path );

	/**
	 * @brief Gets the path to the script list file.
	 * @return Optional containing path or empty if not set.
	 */
	std::optional<fs::path> GetScriptListPath() const;

	/**
	 * @brief Sets the file icon path (used for export or project visuals).
	 * @param path Path to the icon file.
	 */
	void SetFileIconPath( const fs::path& path );

	/**
	 * @brief Gets the icon file path, if available.
	 * @return Optional containing path or empty if not set.
	 */
	std::optional<fs::path> GetFileIconPath() const;

	/**
	 * @brief Maps a specific folder type to a real folder path.
	 * @param eFolderType Folder type enum.
	 * @param path Corresponding filesystem path.
	 * @return True if added successfully.
	 */
	bool AddFolderPath( EProjectFolderType eFolderType, const std::filesystem::path& path );

	/**
	 * @brief Retrieves a folder path by type, if set.
	 * @param eFolderType Folder type to look up.
	 * @return Optional containing path or empty if not found.
	 */
	std::optional<fs::path> TryGetFolderPath( EProjectFolderType eFolderType );

	/**
	 * @brief Gets the pointer to the loaded icon texture, if available.
	 * @return Raw pointer to the texture object.
	 */
	inline const SCION_RENDERING::Texture* GetIconTexturePtr() const { return m_pIconTexture.get(); }

	/** @brief Sets the project name. */
	inline void SetProjectName( const std::string& sProjectName ) { m_sProjectName = sProjectName; }
	/** @brief Gets the project name. */
	inline const std::string& GetProjectName() const { return m_sProjectName; }
	/** @brief Sets the project version string. */
	inline void SetProjectVersion( const std::string& sVersion ) { m_sProjectVersion = sVersion; }
	/** @brief Gets the project version. */
	inline const std::string& GetProjectVersion() const { return m_sProjectVersion; }
	/** @brief Sets the project description. */
	inline void SetProjectDescription( const std::string& sDescription ) { m_sProjectDescription = sDescription; }
	/** @brief Gets the project description. */
	inline const std::string& GetProjectDescription() const { return m_sProjectDescription; }
	/** @brief Sets the copyright notice. */
	inline void SetCopyRightNotice( const std::string& sCopyRightNotice ) { m_sCopyRightNotice = sCopyRightNotice; }
	/** @brief Gets the copyright notice. */
	inline const std::string& GetCopyRightNotice() const { return m_sCopyRightNotice; }

	/**
	 * @brief Gets all project folder mappings.
	 * @return A const reference to the folder type to path map.
	 */
	inline const std::unordered_map<EProjectFolderType, fs::path>& GetProjectPaths() const
	{
		return m_mapProjectFolderPaths;
	}

  private:
	/** @brief Map of folder type enums to their paths on disk. */
	std::unordered_map<EProjectFolderType, fs::path> m_mapProjectFolderPaths;
	/** @brief Root directory path of the project. */
	fs::path m_ProjectPath{};
	/** @brief Optional path to the project metadata file. */
	std::optional<fs::path> m_ProjectFilePath{ std::nullopt };
	/** @brief Path to the main entry Lua script. */
	std::optional<fs::path> m_MainLuaScript{ std::nullopt };
	/** @brief Icon file path (used for packaging or display). */
	std::optional<fs::path> m_FileIconPath{ std::nullopt };
	/** @brief List of Lua scripts used in the project. */
	std::optional<fs::path> m_ScriptListPath{ std::nullopt };
	/** @brief Name of the project. */
	std::string m_sProjectName{};
	/** @brief Semantic version string of the project (e.g., 1.0.0). */
	std::string m_sProjectVersion{ "1.0.0" };
	/** @brief Human-readable description of the project. */
	std::string m_sProjectDescription{};
	/** @brief Copyright or license notice. */
	std::string m_sCopyRightNotice{};
	/** @brief Shared texture pointer for the icon. */
	std::shared_ptr<SCION_RENDERING::Texture> m_pIconTexture{ nullptr };

	/** @brief Whether vertical sync is enabled in the game window. */
	bool m_bUseVSync{ true };
	/** @brief Whether the window is resizable. */
	bool m_bResizable{ false };
	/** @brief Whether the window should be fullscreen. */
	bool m_bFullScreen{ false };
	/** @brief Whether the window is borderless. */
	bool m_bBorderless{ false };
	/** @brief Whether the window supports high-DPI scaling. */
	bool m_bAllowHighDPI{ false };
};

using ProjectInfoPtr = std::shared_ptr<ProjectInfo>;

struct GameConfig
{
	std::string sGameName{};
	std::string sStartupScene{};

	int windowWidth{ 640 };
	int windowHeight{ 480 };
	uint32_t windowFlags{ 0 };

	int cameraWidth{ 640 };
	int cameraHeight{ 480 };
	float cameraScale{ 1.f };

	bool bPhysicsEnabled{ false };
	int32_t positionIterations{ 8 };
	int32_t velocityIterations{ 8 };
	float gravity{ 9.8f };

	bool bPackageAssets{ false };

	void Reset()
	{
		sGameName.clear();
		sStartupScene.clear();

		windowWidth = 640;
		windowHeight = 480;
		windowFlags = 0;

		cameraWidth = 640;
		cameraHeight = 480;
		cameraScale = 1.f;

		bPhysicsEnabled = false;
		positionIterations = 8;
		velocityIterations = 8;
		gravity = 9.8f;

		bPackageAssets = false;
	}
};

} // namespace SCION_CORE
