#include "ScriptCompiler.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "ScionUtilities/HelperUtilities.h"
#include "ScionFilesystem/Utilities/FilesystemUtilities.h"
#include "Logger/Logger.h"

#include <sol/sol.hpp>
#include <filesystem>
#include <algorithm>
#include <array>

#ifdef _WIN32
constexpr const std::string_view FIND_LUAC_COMMAND = "where luac";
#else
constexpr const std::string_view FIND_LUAC_COMMAND = "which luac";
#endif

namespace fs = std::filesystem;

namespace SCION_EDITOR
{

ScriptCompiler::ScriptCompiler()
	: m_sOutFile{}
	, m_LuaFiles{}
	, m_sLuacPath{ std::nullopt }
{
	m_sLuacPath = FindLuaCompiler();
	if ( !m_sLuacPath )
	{
		throw std::runtime_error(
			"ScriptCompiler Error: Lua Compiler (luac) not found on system.\n"
			"Please install the luac compiler and add to your environment, so the editor can find it." );
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
		auto result = lua.safe_script_file( sScriptList );
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

		const std::string sContentPath = fmt::format( "{}content{}", CORE_GLOBALS().GetProjectPath(), PATH_SEPARATOR );

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
	std::string sCommand = fmt::format( "cmd /C \"\"{}\" \"-o\" \"{}\"", *m_sLuacPath, m_sOutFile );
#else
	std::string sCommand = fmt::format( "\"\"{}\" \"-o\" \"{}\"", *m_sLuacPath, m_sOutFile );
#endif // _WIN32

	for ( const auto& file : m_LuaFiles )
	{
		sCommand += " " + std::string{ "\"" + file + "\"" };
	}

	sCommand += "\"";

	const std::string sError = SCION_FILESYSTEM::ExecCmdWithErrorOutput( sCommand );

	if ( !sError.empty() )
	{
		throw std::runtime_error( fmt::format( "ScriptCompiler Error: LuaCompilation failed. Error: {}", sError ) );
	}

	SCION_LOG( "Successfully compiled lua files in [{}]", m_sOutFile );
}

std::optional<std::string> ScriptCompiler::FindLuaCompiler()
{
#ifdef _WIN32
	std::unique_ptr<FILE, decltype( &_pclose )> pipe( _popen( std::string{ FIND_LUAC_COMMAND }.c_str(), "r" ),
													  _pclose );
#else
	std::unique_ptr<FILE, decltype( &pclose )> pipe( popen( std::string{ FIND_LUAC_COMMAND }.c_str(), "r" ), pclose );
#endif

	if ( !pipe )
		return std::nullopt;

	std::string sResult{};
	std::array<char, 256> buffer;

	if ( fgets( buffer.data(), buffer.size(), pipe.get() ) != nullptr )
	{
		sResult = buffer.data();
		sResult.erase( sResult.find_last_not_of( " \n\r" ) + 1 );
	}

	return sResult.empty() ? std::nullopt : std::optional<std::string>{ sResult };
}
} // namespace SCION_EDITOR
