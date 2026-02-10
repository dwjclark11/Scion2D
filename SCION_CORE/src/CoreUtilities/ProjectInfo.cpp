#include "Core/CoreUtilities/ProjectInfo.h"
#include "Rendering/Essentials/Texture.h"
#include "Rendering/Essentials/TextureLoader.h"
#include "Logger/Logger.h"
#include <SDL_mixer.h>

using namespace Scion::Rendering;

namespace Scion::Core
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


bool AudioConfigInfo::UpdateSoundChannels( int numChannels )
{
	if ( allocatedSoundChannels + numChannels > 64)
	{
		SCION_ERROR( "Failed to update sound channels. Max 64 channels are supported." );
		return false;
	}
	else if (allocatedSoundChannels + numChannels < 8)
	{
		SCION_ERROR( "Failed to update sound channels. There must be at least 8 sound channels." );
		return false;
	}

	if ( numChannels > 0 )
		AddChannels( numChannels );
	else if ( numChannels < 0 )
		RemoveChannels( -numChannels );

	Mix_AllocateChannels( allocatedSoundChannels );

	return true;
}

bool AudioConfigInfo::EnableChannelOverride( int channel, bool bEnable )
{
	auto channelItr = mapSoundChannelVolume.find( channel );
	if (channelItr == mapSoundChannelVolume.end())
	{
		SCION_ERROR( "Failed to change sound channel override. Channel[{}] is invalid.", channel );
		return false;
	}

	channelItr->second.first = bEnable;
	return true;
}

bool AudioConfigInfo::SetChannelVolume( int channel, int volume )
{
	auto channelItr = mapSoundChannelVolume.find( channel );
	if ( channelItr == mapSoundChannelVolume.end() )
	{
		SCION_ERROR( "Failed to set sound channel volume. Channel[{}] is invalid.", channel );
		return false;
	}

	channelItr->second.second = volume;
	return true;
}

void AudioConfigInfo::AddChannels( int numChannels )
{
	for (int i = 0; i < numChannels; ++i)
	{
		int channelID{ allocatedSoundChannels + i };
		mapSoundChannelVolume.emplace( channelID, std::make_pair( false, 100 ) );
	}

	allocatedSoundChannels += numChannels;
}

void AudioConfigInfo::RemoveChannels( int numChannels )
{
	for (int i = 0; i < numChannels && allocatedSoundChannels > 0; ++i)
	{
		int channelID{ allocatedSoundChannels - 1 };
		mapSoundChannelVolume.erase( channelID );
		--allocatedSoundChannels;
	}
}

} // namespace Scion::Core
