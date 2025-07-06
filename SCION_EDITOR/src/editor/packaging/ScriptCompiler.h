#pragma once
#include <string>
#include <vector>
#include <optional>

namespace SCION_EDITOR
{
class ScriptCompiler
{
  public:
	/**
	* @brief Constructs the ScriptCompiler and attempts to locate the Lua compiler (luac).
	* @throws std::runtime_error if luac is not available on the system path.
	*/
	ScriptCompiler();
	~ScriptCompiler();

	/*
	 * @brief Reads in a script list, which is a lua table that
	 * lists all the desired scripts, that are added to be compiled.
	 * @param sScript the path of the script list.
	 * @return Returns true if read successfully, false otherwise.
	 */
	bool AddScripts( const std::string& sScriptList );
	bool AddScript( const std::string& sScript );

	/*
	 * @brief Compiles all the lua scripts that were added into
	 * a bytecode luac file.
	 * @throw This will throw an std::runtime_error if there are no scripts,
	 * @throw if any of the scripts don't exist, or if there are logic errors in any
	 * lua script.
	 */
	void Compile();

	inline void SetOutputFileName( const std::string& sOutFile ) { m_sOutFile = sOutFile; }
	inline void ClearScripts() { m_LuaFiles.clear(); }

	private:
	/*
	 * @brief Checks to see if the luac compiler exists on the system.
	 * @return Returns an optional string with the location if successful. Nullopt otherwise.
	 */
	std::optional<std::string> FindLuaCompiler();

  private:
	/* The path to the compiled output luac file. */
	std::string m_sOutFile;
	/* A list of lua scripts to be compiled. */
	std::vector<std::string> m_LuaFiles;
	/* Stores the path to the lua compiler. */
	std::optional<std::string> m_sLuacPath;
};
} // namespace SCION_EDITOR
