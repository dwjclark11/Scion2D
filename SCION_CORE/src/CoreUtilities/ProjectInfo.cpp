#include "Core/CoreUtilities/ProjectInfo.h"
#include "Rendering/Essentials/Texture.h"
#include "Rendering/Essentials/TextureLoader.h"

#include "Logger/Logger.h"

using namespace SCION_RENDERING;

namespace SCION_CORE
{
void ProjectInfo::SetProjectPath( const fs::path& path )
{
	SCION_ASSERT( fs::exists( path ) && "Project Path Must Exist!" );
	m_ProjectPath = path;
}

const fs::path& ProjectInfo::GetProjectPath() const
{
	SCION_ASSERT( fs::exists( m_ProjectPath ) && "Project Path was not set correctly!" );
	return m_ProjectPath;
}

void ProjectInfo::SetProjectFilePath( const fs::path& path )
{
	SCION_ASSERT( fs::exists( path ) && "Project File Path Must Exist!" );
	m_ProjectFilePath = path;
}

std::optional<fs::path> ProjectInfo::GetProjectFilePath() const
{
	return m_ProjectFilePath;
}
void ProjectInfo::SetMainLuaScriptPath( const fs::path& path )
{
	SCION_ASSERT( fs::exists( path ) && "Main Lua Script File Path Must Exist!" );
	m_MainLuaScript = path;
}

std::optional<fs::path> ProjectInfo::GetMainLuaScriptPath() const
{
	return m_MainLuaScript;
}

void ProjectInfo::SetScriptListPath( const fs::path& path )
{
	SCION_ASSERT( fs::exists( path ) && "Script List File Path Must Exist!" );
	m_ScriptListPath = path;
}

std::optional<fs::path> ProjectInfo::GetScriptListPath() const
{
	return m_ScriptListPath;
}

void ProjectInfo::SetFileIconPath( const fs::path& path )
{
	SCION_ASSERT( fs::exists( path ) && "File Icon Path Must Exist!" );
	m_FileIconPath = path;

	// Set the texture
	if (m_pIconTexture)
	{
		m_pIconTexture->Destroy();
		m_pIconTexture.reset();
	}

	m_pIconTexture = TextureLoader::Create( Texture::TextureType::ICON, path.string(), false);
	if (!m_pIconTexture)
	{
		SCION_ERROR( "Failed to load icon texture: [{}]", path.string() );
		return;
	}

	//SCION_ASSERT( m_pIconTexture && "Loading of icon texture failed." );
	SCION_LOG( "Created Icon Texture: [{}]", path.string() );
}

std::optional<fs::path> ProjectInfo::GetFileIconPath() const
{
	return m_FileIconPath;
}

bool ProjectInfo::AddFolderPath( EProjectFolderType eFolderType, const std::filesystem::path& path )
{
	auto [ it, bSuccess] = m_mapProjectFolderPaths.try_emplace( eFolderType, path );
	return bSuccess;
}
std::optional<fs::path> ProjectInfo::TryGetFolderPath( EProjectFolderType eFolderType )
{
	if ( auto it = m_mapProjectFolderPaths.find( eFolderType ); it != m_mapProjectFolderPaths.end() )
	{
		return it->second;
	}

	return std::nullopt;
}
} // namespace SCION_CORE
