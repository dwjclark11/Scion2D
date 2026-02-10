#pragma once

namespace Scion::Core
{
class ProjectInfo;
}

namespace Scion::Editor
{

class ProjectLoader
{
  public:
	/**
	 * @brief Creates a new project directory structure and initializes project metadata.
	 *
	 * This function sets up the standard folder hierarchy for a new project, initializes
	 * the project name and path, and attempts to create a new project file.
	 *
	 * @param sProjectName The name of the new project.
	 * @param sFilepath The base directory path where the project should be created.
	 * @return true if the project is successfully created.
	 * @return false if any step in the project creation process fails.
	 */
	bool CreateNewProject( const std::string& sProjectName, const std::string& sFilepath );

	/**
	 * @brief Loads an existing project from the specified project file.
	 *
	 * Parses the project file, initializes project metadata, validates required folders and
	 * assets, and loads all associated assets (textures, sounds, music, scenes, etc.) into
	 * their respective managers.
	 *
	 * @param sFilepath The full path to the `.s2dprj` project file.
	 * @return true if the project was successfully loaded.
	 * @return false if the file is invalid or any required asset/folder is missing.
	 */
	bool LoadProject( const std::string& sFilepath );

	/**
	 * @brief Saves the current loaded project's data and assets back to the project file.
	 *
	 * Serializes the project metadata, asset paths, scenes, prefabs, and physics configuration
	 * to the project's JSON file. All asset paths are made relative to the content folder.
	 *
	 * @param projectInfo The current project metadata and configuration.
	 * @return true if the project was successfully saved.
	 * @return false if the file could not be written or serialization fails.
	 */
	bool SaveLoadedProject( const Scion::Core::ProjectInfo& projectInfo );

  private:
	/**
	 * @brief Creates the initial `.s2dprj` project file with default values.
	 *
	 * This includes generating a valid JSON project structure, inserting warnings,
	 * and saving initial placeholders for assets and physics settings.
	 *
	 * @param sProjectName The name of the project.
	 * @param sFilepath The full directory path where the project is stored.
	 * @return true if the project file was successfully created.
	 * @return false if any file operation or serialization fails.
	 */
	bool CreateProjectFile( const std::string& sProjectName, const std::string& sFilepath );

	/**
	 * @brief Generates a default `main.lua` file used as the entry point for the project.
	 *
	 * The file is placed under `content/scripts/` and contains stub `update` and `render`
	 * functions required by the engine. This file is automatically created on project setup.
	 *
	 * @param sProjectName The name of the project.
	 * @param sFilepath The base directory path where the project is created.
	 * @return true if the file was created and written successfully.
	 * @return false if the file could not be created.
	 */
	bool CreateMainLuaScript( const std::string& sProjectName, const std::string& sFilepath );

	/**
	 * @brief Creates a `script_list.lua` file under the GameConfig folder if it doesn't exist.
	 *
	 * The file is initialized as empty and its path is stored in the project metadata.
	 *
	 * @return true if the script list file exists or is created successfully.
	 * @return false if the GameConfig folder is not found or the file cannot be created.
	 */
	bool CreateScriptListFile();
};

} // namespace Scion::Editor
