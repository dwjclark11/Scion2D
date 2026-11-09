#include "editor/packaging/ScriptCompiler.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "ScionUtilities/HelperUtilities.h"
#include "ScionFilesystem/Utilities/FilesystemUtilities.h"
#include "Logger/Logger.h"

#include <sol/sol.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <sstream>
#include <system_error>
#include <vector>

namespace Scion::Editor
{

namespace
{
std::vector<std::string> ReadCommandLines( const std::string& sCommand )
{
	std::vector<std::string> lines{};

#ifdef _WIN32
	std::unique_ptr<FILE, decltype( &_pclose )> pipe( _popen( sCommand.c_str(), "r" ), _pclose );
#else
	std::unique_ptr<FILE, decltype( &pclose )> pipe( popen( sCommand.c_str(), "r" ), pclose );
#endif

	if ( !pipe )
		return lines;

	std::array<char, 512> buffer;

	while ( fgets( buffer.data(), static_cast<int>( buffer.size() ), pipe.get() ) != nullptr )
	{
		std::string sLine{ buffer.data() };
		sLine.erase( sLine.find_last_not_of( " \n\r" ) + 1 );

		if ( !sLine.empty() )
			lines.push_back( sLine );
	}

	return lines;
}

std::vector<std::string> BuildLuacNames()
{
	const std::string sMajor{ LUA_VERSION_MAJOR };
	const std::string sMinor{ LUA_VERSION_MINOR };

	return { "luac" + sMajor + sMinor,
			 "luac" + sMajor + "." + sMinor,
			 "luac-" + sMajor + "." + sMinor,
			 "luac" + sMajor + "-" + sMinor,
			 "luac" };
}

std::string GetPathEnv()
{
#ifdef _WIN32
	char* pBuffer{ nullptr };
	std::size_t size{ 0 };

	if ( _dupenv_s( &pBuffer, &size, "PATH" ) != 0 || pBuffer == nullptr )
		return {};

	std::string sPath{ pBuffer };
	free( pBuffer );
	return sPath;
#else
	const char* pPath{ std::getenv( "PATH" ) };
	return pPath ? std::string{ pPath } : std::string{};
#endif
}

std::vector<std::string> FindLuacCandidates()
{
	std::vector<std::string> found{};

	const std::string sPathEnv{ GetPathEnv() };

	if ( sPathEnv.empty() )
		return found;

#ifdef _WIN32
	constexpr char cSeparator{ ';' };
	const std::vector<std::string> extensions{ ".exe", "" };
#else
	constexpr char cSeparator{ ':' };
	const std::vector<std::string> extensions{ "" };
#endif

	const auto names{ BuildLuacNames() };

	std::stringstream ss{ sPathEnv };
	std::string sDir{};

	while ( std::getline( ss, sDir, cSeparator ) )
	{
		if ( sDir.empty() )
			continue;

		for ( const auto& sName : names )
		{
			for ( const auto& sExt : extensions )
			{
				std::error_code ec{};
				const fs::path path{ fs::path{ sDir } / ( sName + sExt ) };

				if ( !fs::is_regular_file( path, ec ) )
					continue;

				const std::string sFull{ path.string() };

				if ( std::find( found.begin(), found.end(), sFull ) == found.end() )
					found.push_back( sFull );
			}
		}
	}

	return found;
}
} // namespace

ScriptCompiler::ScriptCompiler()
	: m_sOutFile{}
	, m_LuaFiles{}
	, m_sLuacPath{ std::nullopt }
{
	m_sLuacPath = FindLuaCompiler();
	if ( !m_sLuacPath )
	{
		throw std::runtime_error(
			fmt::format( "ScriptCompiler Error: Lua Compiler (luac) for Lua {}.{} not found on system.\n"
						 "Please install a matching luac (luac{}{} or luac) and add it to your environment, "
						 "so the editor can find it.",
						 LUA_VERSION_MAJOR,
						 LUA_VERSION_MINOR,
						 LUA_VERSION_MAJOR,
						 LUA_VERSION_MINOR ) );
	}
}

ScriptCompiler::~ScriptCompiler() = default;

bool ScriptCompiler::AddScripts( const std::string& sScriptList )
{
	if ( !fs::exists( fs::path{ sScriptList } ) )
	{
		SCION_ERROR( "Failed to add scripts. Script list file [{}] is invalid.", sScriptList );
		return false;
	}

	sol::state lua{};

	try
	{
		auto result = lua.safe_script_file( sScriptList, sol::script_pass_on_error );
		if ( !result.valid() )
		{
			sol::error error = result;
			throw error;
		}

		sol::optional<sol::table> optScriptList = lua[ "ScriptList" ];
		if ( !optScriptList )
		{
			SCION_ERROR( "Failed to add scripts. [{}] is missing \"ScriptList\" table or is invalid.", sScriptList );
			return false;
		}

		const std::string sContentPath =
			fmt::format( "{}{}content{}", CORE_GLOBALS().GetProjectPath(), PATH_SEPARATOR, PATH_SEPARATOR );

		for ( const auto& [ _, script ] : *optScriptList )
		{
			std::string sFilepath{ fmt::format( "{}{}", sContentPath, script.as<std::string>() ) };
			if ( !fs::exists( fs::path{ sFilepath } ) )
			{
				SCION_ERROR( "Failed to add script: [{}] - File does not exist.", sFilepath );
				return false;
			}

			m_LuaFiles.push_back( sFilepath );
		}
	}
	catch ( const sol::error& error )
	{
		SCION_ERROR( "Failed to add scripts. Error: {}", error.what() );
		return false;
	}

	return true;
}

bool ScriptCompiler::AddScript( const std::string& sScript )
{
	if ( !fs::exists( fs::path{ sScript } ) )
	{
		SCION_ERROR( "Failed to add script [{}] - File does not exist.", sScript );
		return false;
	}

	m_LuaFiles.push_back( sScript );

	return true;
}

void ScriptCompiler::Compile()
{
	if ( m_LuaFiles.empty() )
	{
		throw std::invalid_argument( "ScriptCompiler Error: No Lua Files Provided for compilation." );
	}

	auto notExist =
		std::ranges::find_if( m_LuaFiles, []( const std::string& sFile ) { return !fs::exists( fs::path{ sFile } ); } );

	if ( notExist != m_LuaFiles.end() )
	{
		throw std::runtime_error( "ScriptCompiler Error: File not found - " + *notExist );
	}

#ifdef _WIN32
	std::string sCommand = fmt::format( "cmd /C \"\"{}\" -o \"{}\"", *m_sLuacPath, m_sOutFile );
#else
	std::string sCommand = fmt::format( "\"{}\" -o \"{}\"", *m_sLuacPath, m_sOutFile );
#endif // _WIN32

	for ( const auto& file : m_LuaFiles )
	{
		sCommand += " \"" + file + "\"";
	}

#ifdef _WIN32
	sCommand += "\"";
#endif // _WIN32

	const std::string sError = Scion::Filesystem::ExecCmdWithErrorOutput( sCommand );

	if ( !sError.empty() )
	{
		throw std::runtime_error( fmt::format( "ScriptCompiler Error: LuaCompilation failed. Error: {}", sError ) );
	}

	SCION_LOG( "Successfully compiled lua files in [{}]", m_sOutFile );
}

std::optional<std::string> ScriptCompiler::FindLuaCompiler()
{
	const std::string sWanted{ std::string{ "Lua " } + LUA_VERSION_MAJOR + "." + LUA_VERSION_MINOR };

	for ( const auto& sPath : FindLuacCandidates() )
	{
#ifdef _WIN32
		const std::string sCommand{ "\"\"" + sPath + "\" -v 2>&1\"" };
#else
		const std::string sCommand{ "\"" + sPath + "\" -v 2>&1" };
#endif

		const auto output{ ReadCommandLines( sCommand ) };

		if ( !output.empty() && output.front().rfind( sWanted, 0 ) == 0 )
			return sPath;
	}

	return std::nullopt;
}

} // namespace Scion::Editor
