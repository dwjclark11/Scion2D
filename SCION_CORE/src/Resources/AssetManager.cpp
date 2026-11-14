#include "Core/Resources/AssetManager.h"
#include "Core/Resources/fonts/default_fonts.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/Prefab.h"
#include "Core/ECS/Registry.h"

#include <Rendering/Essentials/TextureLoader.h>
#include <Rendering/Essentials/ShaderLoader.h>
#include <Rendering/Essentials/FontLoader.h>
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>
#include <Rendering/Essentials/Font.h>

#include <Sounds/Essentials/Audio.hpp>
#include <Sounds/AudioPlayer/AudioPlayer.hpp>

#include <ScionUtilities/ScionUtilities.h>
#include <ScionUtilities/SDL_Wrappers.h>
#include <Logger/Logger.h>
#include <SDL3_image/SDL_image.h>

namespace fs = std::filesystem;
using namespace std::chrono_literals;

namespace Scion::Resources
{
AssetManager::AssetManager( bool bEnableFilewatcher )
	: m_bFileWatcherRunning{ bEnableFilewatcher }
{
	if ( bEnableFilewatcher )
	{
		m_WatchThread = std::jthread( &AssetManager::FileWatcher, this );
	}

#ifdef IN_SCION_EDITOR
	m_mapCursors.emplace( "default", MakeSharedFromSDLType<Cursor>( SDL_GetDefaultCursor() ) );
#endif
}

AssetManager::~AssetManager()
{
	m_bFileWatcherRunning = false;
	if ( m_WatchThread.joinable() )
	{
		m_WatchThread.join();
	}
}

bool AssetManager::CreateDefaultFonts()
{
	if ( !AddFontFromMemory( "pixel-32", CoreFonts::g_PixelFont ) )
	{
		SCION_ERROR( "Failed to create pixel font." );
		return false;
	}

	if ( !AddFontFromMemory( "roboto-bold-32", CoreFonts::g_PixelFont ) )
	{
		SCION_ERROR( "Failed to create roboto font." );
		return false;
	}

	// TODO: Add more default fonts.

	return true;
}

bool AssetManager::AddTexture( const std::string& textureName, const std::string& texturePath, bool pixelArt,
							   bool bTileset )
{
	// Check to see if the texture already exists
	if ( m_mapTextures.find( textureName ) != m_mapTextures.end() )
	{
		SCION_ERROR( "Failed to add texture [{0}] -- Already exists!", textureName );
		return false;
	}

	auto pTexture = Scion::Rendering::TextureLoader::Create( pixelArt ? Scion::Rendering::Texture::TextureType::PIXEL
																	  : Scion::Rendering::Texture::TextureType::BLENDED,
															 texturePath,
															 bTileset );

	if ( !pTexture )
	{
		SCION_ERROR( "Failed to load texture [{0}] at path [{1}]", textureName, texturePath );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapTextures.emplace( textureName, std::move( pTexture ) );

	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };

		fs::path path{ texturePath };
		auto lastWrite = fs::last_write_time( path );
		if ( Scion::Utilities::CheckContainsValue(
				 m_FilewatchParams, [ & ]( const auto& params ) { return params.sFilepath == texturePath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = textureName,
															  .sFilepath = texturePath,
															  .lastWrite = lastWrite,
															  .eType = Scion::Utilities::AssetType::TEXTURE } );
		}
	}

	return bSuccess;
}

bool AssetManager::AddTextureFromMemory( const std::string& textureName, const unsigned char* imageData, size_t length,
										 bool pixelArt, bool bTileset )
{
	// Check to see if the Texture already exist
	if ( m_mapTextures.contains( textureName ) )
	{
		SCION_ERROR( "AssetManager: Texture [{}] -- Already exists!", textureName );
		return false;
	}

	auto pTexture = Scion::Rendering::TextureLoader::CreateFromMemory( imageData, length, !pixelArt, bTileset );

	// Load the texture
	if ( !pTexture )
	{
		SCION_ERROR( "Unable to load texture [{}] from memory!", textureName );
		return false;
	}

	// Insert the texture into the map
	auto [ itr, bSuccess ] = m_mapTextures.emplace( textureName, std::move( pTexture ) );

	return bSuccess;
}

Scion::Rendering::Texture* AssetManager::GetTexture( const std::string& textureName )
{
	auto texItr = m_mapTextures.find( textureName );
	if ( texItr == m_mapTextures.end() )
	{
		SCION_ERROR( "Failed to get texture [{0}] -- Does not exist!", textureName );
		return nullptr;
	}

	return texItr->second.get();
}

std::vector<std::string> AssetManager::GetTilesetNames() const
{
	return Scion::Utilities::GetKeys( m_mapTextures, []( const auto& pair ) { return pair.second->IsTileset(); } );
}

bool AssetManager::AddFont( const std::string& fontName, const std::string& fontPath, float fontSize )
{
	if ( m_mapFonts.contains( fontName ) )
	{
		SCION_ERROR( "Failed to add font [{0}] -- Already Exists!", fontName );
		return false;
	}

	auto pFont = Scion::Rendering::FontLoader::Create( fontPath, fontSize );

	if ( !pFont )
	{
		SCION_ERROR( "Failed to add font [{}] at path [{}] -- to the asset manager!", fontName, fontPath );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapFonts.emplace( fontName, std::move( pFont ) );

	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };

		fs::path path{ fontPath };
		auto lastWrite = fs::last_write_time( path );
		if ( Scion::Utilities::CheckContainsValue(
				 m_FilewatchParams, [ & ]( const auto& params ) { return params.sFilepath == fontPath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = fontName,
															  .sFilepath = fontPath,
															  .lastWrite = lastWrite,
															  .eType = Scion::Utilities::AssetType::FONT } );
		}
	}

	return bSuccess;
}

bool AssetManager::AddFontFromMemory( const std::string& fontName, unsigned char* fontData, float fontSize )
{

	if ( m_mapFonts.contains( fontName ) )
	{
		SCION_ERROR( "Failed to add font [{0}] -- Already Exists!", fontName );
		return false;
	}

	auto pFont = Scion::Rendering::FontLoader::CreateFromMemory( fontData, fontSize );

	if ( !pFont )
	{
		SCION_ERROR( "Failed to add font [{0}] from memory -- to the asset manager!", fontName );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapFonts.emplace( fontName, std::move( pFont ) );

	return bSuccess;
}

Scion::Rendering::Font* AssetManager::GetFont( const std::string& fontName )
{
	auto fontItr = m_mapFonts.find( fontName );
	if ( fontItr == m_mapFonts.end() )
	{
		SCION_ERROR( "Failed to get font [{0}] -- Does not exist!", fontName );
		return nullptr;
	}

	return fontItr->second.get();
}

bool AssetManager::AddShader( const std::string& shaderName, const std::string& vertexPath,
							  const std::string& fragmentPath )
{
	// Check to see if the shader already exists
	if ( m_mapShader.contains( shaderName ) )
	{
		SCION_ERROR( "Failed to add shader [{0}] -- Already Exists!", shaderName );
		return false;
	}

	// Create and load the shader
	auto pShader = Scion::Rendering::ShaderLoader::Create( vertexPath, fragmentPath );

	if ( !pShader )
	{
		SCION_ERROR( "Failed to load Shader [{0}] at vert path [{1}] and frag path [{2}]",
					 shaderName,
					 vertexPath,
					 fragmentPath );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapShader.emplace( shaderName, std::move( pShader ) );

	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };

		fs::path pathVert{ vertexPath };
		auto lastWriteVert = fs::last_write_time( pathVert );
		if ( Scion::Utilities::CheckContainsValue(
				 m_FilewatchParams, [ & ]( const auto& params ) { return params.sFilepath == vertexPath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = shaderName + "_vert",
															  .sFilepath = vertexPath,
															  .lastWrite = lastWriteVert,
															  .eType = Scion::Utilities::AssetType::SHADER } );
		}

		fs::path pathFrag{ fragmentPath };
		auto lastWriteFrag = fs::last_write_time( pathFrag );
		if ( Scion::Utilities::CheckContainsValue(
				 m_FilewatchParams, [ & ]( const auto& params ) { return params.sFilepath == fragmentPath; } ) )
		{
			m_FilewatchParams.emplace_back( AssetWatchParams{ .sAssetName = shaderName + "_frag",
															  .sFilepath = fragmentPath,
															  .lastWrite = lastWriteFrag,
															  .eType = Scion::Utilities::AssetType::SHADER } );
		}
	}
	return bSuccess;
}

bool AssetManager::AddShaderFromMemory( const std::string& shaderName, const char* vertexShader,
										const char* fragShader )
{
	if ( m_mapShader.contains( shaderName ) )
	{
		SCION_ERROR( "Failed to add shader - [{0}] -- Already exists!", shaderName );
		return false;
	}

	auto pShader = Scion::Rendering::ShaderLoader::CreateFromMemory( vertexShader, fragShader );
	auto [ itr, bSuccess ] = m_mapShader.emplace( shaderName, std::move( pShader ) );

	return bSuccess;
}

Scion::Rendering::Shader* AssetManager::GetShader( const std::string& shaderName )
{
	auto shaderItr = m_mapShader.find( shaderName );
	if ( shaderItr == m_mapShader.end() )
	{
		SCION_ERROR( "Failed to get shader [{0}] -- Does not exist!", shaderName );
		return nullptr;
	}

	return shaderItr->second.get();
}

bool AssetManager::AddAudio( const std::string& audioName, const std::string& filepath, Scion::Sounds::AudioType eType )
{
	// Check to see if the shader already exists
	if ( m_mapAudio.contains( audioName ) )
	{
		SCION_ERROR( "Failed to add Audio [{0}] -- Already Exists!", audioName );
		return false;
	}

	MIX_Audio* pAudio = MIX_LoadAudio( nullptr, filepath.c_str(), false );
	if ( !pAudio )
	{
		SCION_ERROR( "Failed to add audio. [{}]. Error: {}", audioName, SDL_GetError() );
		return false;
	}

	return m_mapAudio.emplace( audioName, std::make_unique<Scion::Sounds::Audio>( pAudio, eType, filepath ) ).second;
}

bool AssetManager::AddAudioFromMemory( const std::string& audioName, const unsigned char* audioData, size_t dataSize,
									   Scion::Sounds::AudioType eType )
{
	// Check to see if the shader already exists
	if ( m_mapAudio.contains( audioName ) )
	{
		SCION_ERROR( "Failed to add Audio [{0}] -- Already Exists!", audioName );
		return false;
	}

	SDL_IOStream* pStream = SDL_IOFromConstMem( audioData, dataSize );
	if ( !pStream )
	{
		SCION_ERROR( "Failed to add Audio from data. [{}]", SDL_GetError() );
		return false;
	}

	MIX_Audio* pAudio = MIX_LoadAudio_IO( nullptr, pStream, false, true );
	if ( !pAudio )
	{
		SCION_ERROR( "Failed to add audio from memory. [{}]. Error: {}", audioName, SDL_GetError() );
		return false;
	}

	return m_mapAudio.emplace( audioName, std::make_unique<Scion::Sounds::Audio>( pAudio, eType, "From Memory" ) )
		.second;
}

Scion::Sounds::Audio* AssetManager::GetAudio( const std::string& audioName )
{
	auto audioItr = m_mapAudio.find( audioName );
	if ( audioItr == m_mapAudio.end() )
	{
		SCION_ERROR( "Failed to get audio [{0}] -- Does not exist!", audioName );
		return nullptr;
	}

	return audioItr->second.get();
}

std::string AssetManager::GetAssetFilepath( const std::string& sAssetName, Scion::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Scion::Utilities::AssetType::TEXTURE: {
		auto itr = m_mapTextures.find( sAssetName );
		return itr != m_mapTextures.end() ? itr->second->GetPath() : std::string{};
	}
	case Scion::Utilities::AssetType::FONT: {
		auto itr = m_mapFonts.find( sAssetName );
		return itr != m_mapFonts.end() ? itr->second->GetFilename() : std::string{};
	}
	case Scion::Utilities::AssetType::MUSIC:
	case Scion::Utilities::AssetType::SOUNDFX: {
		auto itr = m_mapAudio.find( sAssetName );
		return itr != m_mapAudio.end() ? itr->second->GetFilename() : std::string{};
	}
	case Scion::Utilities::AssetType::PREFAB: {
		auto itr = m_mapPrefabs.find( sAssetName );
		return itr != m_mapPrefabs.end() ? itr->second->GetFilepath() : std::string{};
	}
	}

	return {};
}

bool AssetManager::AddPrefab( const std::string& sPrefabName, std::unique_ptr<Scion::Core::Prefab> pPrefab )
{
	if ( m_mapPrefabs.contains( sPrefabName ) )
	{
		SCION_ERROR( "Failed to add prefab [{}] -- Already exists in AssetManager.", sPrefabName );
		return false;
	}

	auto [ itr, bSuccess ] = m_mapPrefabs.emplace( sPrefabName, std::move( pPrefab ) );
	return bSuccess;
}

Scion::Core::Prefab* AssetManager::GetPrefab( const std::string& sPrefabName )
{
	auto prefabItr = m_mapPrefabs.find( sPrefabName );
	if ( prefabItr == m_mapPrefabs.end() )
	{
		SCION_ERROR( "Failed to get Prefab [{}] -- Does Not exist!", sPrefabName );
		return nullptr;
	}

	return prefabItr->second.get();
}

#ifdef IN_SCION_EDITOR

bool AssetManager::AddCursor( const std::string& sCursorName, const std::string& sCursorPath )
{
	return false;
}

bool AssetManager::AddCursorFromMemory( const std::string& sCursorName, unsigned char* cursorData, size_t dataSize )
{
	if ( m_mapCursors.contains( sCursorName ) )
	{
		SCION_ERROR( "Failed to add Cursor [{}] - Already exists.", sCursorName );
		return false;
	}

	SDL_IOStream* stream = SDL_IOFromConstMem( cursorData, dataSize );
	if ( !stream )
	{
		SCION_ERROR( "Failed to add cursor. [{}]", SDL_GetError() );
		return false;
	}

	SDL_Surface* pSurface = SDL_LoadPNG_IO( stream, true );
	if ( !pSurface )
	{
		SCION_ERROR( "Failed to add cursor. [{}]", SDL_GetError() );
		return false;
	}

	SDL_Cursor* pCursor = SDL_CreateColorCursor( pSurface, pSurface->w / 2, pSurface->h / 2 );

	if ( !pCursor )
	{
		SCION_ERROR( "Failed to add cursor. [{}]", SDL_GetError() );
		return false;
	}

	SDL_DestroySurface( pSurface );

	return m_mapCursors.emplace( sCursorName, MakeSharedFromSDLType<Cursor>( pCursor ) ).second;
}

SDL_Cursor* AssetManager::GetCursor( const std::string& sCursorName )
{
	auto cursorItr = m_mapCursors.find( sCursorName );
	if ( cursorItr == m_mapCursors.end() )
	{
		SCION_ERROR( "Failed to get Cursor [{}] -- Does Not exist!", sCursorName );
		return nullptr;
	}

	return cursorItr->second.get();
}

#endif

std::vector<std::string> AssetManager::GetAssetKeyNames( Scion::Utilities::AssetType eAssetType ) const
{
	switch ( eAssetType )
	{
	case Scion::Utilities::AssetType::TEXTURE:
		return Scion::Utilities::GetKeys( m_mapTextures,
										  []( const auto& pair ) { return !pair.second->IsEditorTexture(); } );
	case Scion::Utilities::AssetType::FONT: return Scion::Utilities::GetKeys( m_mapFonts );
	case Scion::Utilities::AssetType::MUSIC:
		return Scion::Utilities::GetKeys(
			m_mapAudio, []( const auto& pair ) { return pair.second->GetType() == Scion::Sounds::AudioType::Music; } );
	case Scion::Utilities::AssetType::SOUNDFX:
		return Scion::Utilities::GetKeys( m_mapAudio, []( const auto& pair ) {
			return pair.second->GetType() == Scion::Sounds::AudioType::Soundfx;
		} );
	case Scion::Utilities::AssetType::PREFAB: return Scion::Utilities::GetKeys( m_mapPrefabs );
	default: SCION_ASSERT( false && "Cannot get this type!" );
	}

	return std::vector<std::string>{};
}

bool AssetManager::ChangeAssetName( const std::string& sOldName, const std::string& sNewName,
									Scion::Utilities::AssetType eAssetType )
{
	bool bSuccess{ false };

	switch ( eAssetType )
	{
	case Scion::Utilities::AssetType::TEXTURE:
		bSuccess = Scion::Utilities::KeyChange( m_mapTextures, sOldName, sNewName );
		break;
	case Scion::Utilities::AssetType::FONT:
		bSuccess = Scion::Utilities::KeyChange( m_mapFonts, sOldName, sNewName );
		break;
	case Scion::Utilities::AssetType::MUSIC:
	case Scion::Utilities::AssetType::SOUNDFX:
		bSuccess = Scion::Utilities::KeyChange( m_mapAudio, sOldName, sNewName );
		break;
	default: SCION_ASSERT( false && "Cannot get this type!" ); break;
	}

	// If we are using the filewatcher, we need to also ensure to adjust the name
	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };
		auto fileItr = std::ranges::find_if( m_FilewatchParams,
											 [ & ]( const auto& param ) { return param.sAssetName == sOldName; } );

		if ( fileItr != m_FilewatchParams.end() )
		{
			fileItr->sAssetName = sNewName;
		}
	}

	return bSuccess;
}

bool AssetManager::CheckHasAsset( const std::string& sNameCheck, Scion::Utilities::AssetType eAssetType )
{
	switch ( eAssetType )
	{
	case Scion::Utilities::AssetType::TEXTURE: return m_mapTextures.contains( sNameCheck );
	case Scion::Utilities::AssetType::FONT: return m_mapFonts.contains( sNameCheck );
	case Scion::Utilities::AssetType::MUSIC:
	case Scion::Utilities::AssetType::SOUNDFX: return m_mapAudio.contains( sNameCheck );
	case Scion::Utilities::AssetType::PREFAB: return m_mapPrefabs.contains( sNameCheck );
	default: SCION_ASSERT( false && "Cannot get this type!" );
	}

	return false;
}

bool AssetManager::DeleteAsset( const std::string& sAssetName, Scion::Utilities::AssetType eAssetType )
{
	bool bSuccess{ false };

	switch ( eAssetType )
	{
	case Scion::Utilities::AssetType::TEXTURE:
		bSuccess = std::erase_if( m_mapTextures, [ & ]( const auto& pair ) { return pair.first == sAssetName; } ) > 0;
		break;
	case Scion::Utilities::AssetType::FONT:
		bSuccess = std::erase_if( m_mapFonts, [ & ]( const auto& pair ) { return pair.first == sAssetName; } ) > 0;
		break;
	case Scion::Utilities::AssetType::SOUNDFX:
	case Scion::Utilities::AssetType::MUSIC:
		bSuccess = std::erase_if( m_mapAudio, [ & ]( const auto& pair ) { return pair.first == sAssetName; } ) > 0;
		break;
	case Scion::Utilities::AssetType::PREFAB: { // Prefabs contain files that must be cleaned up
		if ( auto pPrefab = GetPrefab( sAssetName ) )
		{
			if ( !Scion::Core::PrefabCreator::DeletePrefab( *pPrefab ) )
			{
				SCION_ERROR( "Failed to delete prefab [{}]", sAssetName );
				return false;
			}

			bSuccess = m_mapPrefabs.erase( sAssetName ) > 0;
			break;
		}

		SCION_ERROR( "Failed to delete prefab [{}] - Does not exist in asset manager.", sAssetName );
		return false;
	}
	default: SCION_ASSERT( false && "Cannot get this type!" );
	}

	// If the file watcher is enabled, we need to remove the
	// file from being watched.
	if ( m_bFileWatcherRunning && bSuccess )
	{
		std::lock_guard lock{ m_AssetMutex };
		bool bErased = std::erase_if( m_FilewatchParams,
									  [ & ]( const auto& param ) { return param.sAssetName == sAssetName; } ) > 0;

		if ( !bErased )
		{
			SCION_WARN( "Failed to erase [{}] from File Watcher Params. - Must not be present.", sAssetName );
			// Non-fatal error.
		}
	}

	if ( bSuccess )
	{
		SCION_LOG( "Deleted asset [{}]", sAssetName );
	}

	return bSuccess;
}

bool AssetManager::DeleteAssetFromPath( const std::string& sAssetPath )
{
	auto textureItr = std::ranges::find_if(
		m_mapTextures, [ & ]( const auto& pair ) { return pair.second->GetPath() == sAssetPath; } );

	if ( textureItr != m_mapTextures.end() )
	{
		std::string sTextureName{ textureItr->first };
		return DeleteAsset( sTextureName, Scion::Utilities::AssetType::TEXTURE );
	}

	auto audioItr = std::ranges::find_if(
		m_mapAudio, [ & ]( const auto& pair ) { return pair.second->GetFilename() == sAssetPath; } );

	if ( audioItr != m_mapAudio.end() )
	{
		std::string sMusicName{ audioItr->first };

		if ( audioItr->second->GetType() == Scion::Sounds::AudioType::Music )
		{
			return DeleteAsset( sMusicName, Scion::Utilities::AssetType::MUSIC );
		}
		else if ( audioItr->second->GetType() == Scion::Sounds::AudioType::Soundfx )
		{
			return DeleteAsset( sMusicName, Scion::Utilities::AssetType::SOUNDFX );
		}
	}

	return true;
}

void AssetManager::CreateLuaAssetManager( sol::state& lua )
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	lua.new_usertype<AssetManager>(
		"AssetManager",
		sol::no_constructor,
		"addTexture",
		sol::overload(
			[ & ]( const std::string& assetName, const std::string& filepath, bool pixel_art ) {
				return assetManager.AddTexture( assetName, filepath, pixel_art, false );
			},
			[ & ]( const std::string& assetName, const std::string& filepath, bool pixel_art, bool bTileset ) {
				return assetManager.AddTexture( assetName, filepath, pixel_art, bTileset );
			} ),
		"addAudio",
		[ & ]( const std::string& audioName, const std::string& filename ) {
			return assetManager.AddAudio( audioName, filename, Scion::Sounds::AudioType::None );
		},
		"addFont",
		[ & ]( const std::string& fontName, const std::string& fontPath, float fontSize ) {
			return assetManager.AddFont( fontName, fontPath, fontSize );
		} );
}
void AssetManager::Update()
{
	std::shared_lock sharedLock{ m_AssetMutex };
	auto dirtyView = m_FilewatchParams | std::views::filter( []( const auto& param ) { return param.bDirty; } );

	if ( !dirtyView.empty() )
	{
		sharedLock.unlock();
		std::unique_lock lock{ m_AssetMutex };
		for ( auto& param : dirtyView )
		{
			ReloadAsset( param );
			param.bDirty = false;
		}
	}
}

void AssetManager::FileWatcher()
{
	while ( m_bFileWatcherRunning )
	{
		std::this_thread::sleep_for( 2s );

		for ( auto& fileParam : m_FilewatchParams )
		{
			std::shared_lock sharedLock{ m_AssetMutex };
			fs::path path{ fileParam.sFilepath };
			if ( !fs::exists( path ) )
				continue;

			if ( fileParam.lastWrite != fs::last_write_time( path ) )
			{
				sharedLock.unlock();
				std::unique_lock lock{ m_AssetMutex };
				fileParam.bDirty = true;
			}
		}
	}
}

void AssetManager::ReloadAsset( const AssetWatchParams& assetParams )
{
	switch ( assetParams.eType )
	{
	case Scion::Utilities::AssetType::TEXTURE: ReloadTexture( assetParams.sAssetName ); break;
	case Scion::Utilities::AssetType::FONT: ReloadFont( assetParams.sAssetName ); break;
	case Scion::Utilities::AssetType::SHADER: ReloadShader( assetParams.sAssetName ); break;
	}
}

void AssetManager::ReloadTexture( const std::string& sTextureName )
{
	auto fileParamItr = std::ranges::find_if( m_FilewatchParams,
											  [ & ]( const auto& param ) { return param.sAssetName == sTextureName; } );

	if ( fileParamItr == m_FilewatchParams.end() )
	{
		SCION_ERROR( "Trying to reload a texture that has not been loaded?" );
		return;
	}

	// We are assuming that the texture is in the map.
	// Could potentially cause a crash, will look more into this.
	auto& pTexture = m_mapTextures[ sTextureName ];

	fileParamItr->lastWrite = fs::last_write_time( fs::path{ pTexture->GetPath() } );
	// Delete the old texture and then reload
	auto id = pTexture->GetID();
	glDeleteTextures( 1, &id );

	auto pNewTexture =
		Scion::Rendering::TextureLoader::Create( pTexture->GetType(), pTexture->GetPath(), pTexture->IsTileset() );

	pTexture = std::move( pNewTexture );
	SCION_LOG( "Reloaded texture: {}", sTextureName );
}

void AssetManager::ReloadFont( const std::string& sFontName )
{
	auto fileParamItr =
		std::ranges::find_if( m_FilewatchParams, [ & ]( const auto& param ) { return param.sAssetName == sFontName; } );

	if ( fileParamItr == m_FilewatchParams.end() )
	{
		SCION_ERROR( "Trying to music that has not been loaded?" );
		return;
	}

	fileParamItr->lastWrite = fs::last_write_time( fs::path{ fileParamItr->sFilepath } );

	auto& pFont = m_mapFonts[ sFontName ];
	float fontSize = pFont->GetFontSize();

	if ( !DeleteAsset( sFontName, Scion::Utilities::AssetType::FONT ) )
	{
		SCION_ERROR( "Failed to Reload SoundFx: {}", sFontName );
		return;
	}

	if ( !AddFont( sFontName, fileParamItr->sFilepath, fontSize ) )
	{
		SCION_ERROR( "Failed to Reload SoundFx: {}", sFontName );
		return;
	}

	SCION_LOG( "Reloaded Font: {}", sFontName );
}

void AssetManager::ReloadShader( const std::string& sShaderName )
{
	// Shader watch params use the convention sAssetName = shaderName + "_vert"
	// or shaderName + "_frag". Strip the suffix to find the base shader name.
	// We reload the full shader (both vert and frag) whenever either file changes.

	// Derive the base shader name by stripping the _vert / _frag suffix.
	std::string sBaseShaderName = sShaderName;
	bool bHasSuffix = false;

	if ( sShaderName.ends_with( "_vert" ) )
	{
		sBaseShaderName = sShaderName.substr( 0, sShaderName.size() - 5 );
		bHasSuffix = true;
	}
	else if ( sShaderName.ends_with( "_frag" ) )
	{
		sBaseShaderName = sShaderName.substr( 0, sShaderName.size() - 5 );
		bHasSuffix = true;
	}

	if ( !bHasSuffix )
	{
		SCION_WARN( "AssetManager::ReloadShader -- unexpected shader watch name [{}]. "
				  "Expected <name>_vert or <name>_frag.",
				  sShaderName );
		return;
	}

	// Find the vert and frag watch params for this shader.
	auto vertParamItr = std::ranges::find_if(
		m_FilewatchParams, [ & ]( const auto& param ) { return param.sAssetName == sBaseShaderName + "_vert"; } );

	auto fragParamItr = std::ranges::find_if(
		m_FilewatchParams, [ & ]( const auto& param ) { return param.sAssetName == sBaseShaderName + "_frag"; } );

	if ( vertParamItr == m_FilewatchParams.end() || fragParamItr == m_FilewatchParams.end() )
	{
		SCION_ERROR( "AssetManager::ReloadShader -- could not find both watch params for [{}].", sBaseShaderName );
		return;
	}

	const std::string sVertPath = vertParamItr->sFilepath;
	const std::string sFragPath = fragParamItr->sFilepath;

	// Find the existing shader to check whether it is an engine shader.
	auto shaderItr = m_mapShader.find( sBaseShaderName );
	if ( shaderItr == m_mapShader.end() )
	{
		SCION_ERROR( "AssetManager::ReloadShader -- shader [{}] not in map.", sBaseShaderName );
		return;
	}

	const bool bIsEngineShader = shaderItr->second->IsEngineShader();

	// Compile the new shader before removing the old one so we do not
	// end up with no shader at all on a compile failure.
	auto pNewShader = Scion::Rendering::ShaderLoader::Create( sVertPath, sFragPath );
	if ( !pNewShader )
	{
		SCION_ERROR( "AssetManager::ReloadShader -- recompile failed for [{}]. Keeping old shader.", sBaseShaderName );
		return;
	}

	if ( bIsEngineShader )
		pNewShader->SetAsEngineShader();

	// Swap in the new shader.
	m_mapShader[ sBaseShaderName ] = std::move( pNewShader );

	// Update lastWrite on both watch params so the watcher does not
	// immediately flag them dirty again on the next tick.
	fs::path vertPath{ sVertPath };
	fs::path fragPath{ sFragPath };

	if ( fs::exists( vertPath ) )
		vertParamItr->lastWrite = fs::last_write_time( vertPath );
	if ( fs::exists( fragPath ) )
		fragParamItr->lastWrite = fs::last_write_time( fragPath );

	// Clear the dirty flag on the counterpart param so both
	// vert and frag dirty flags are consumed by one reload.
	if ( sShaderName.ends_with( "_vert" ) )
		fragParamItr->bDirty = false;
	else
		vertParamItr->bDirty = false;

	SCION_LOG( "AssetManager: Reloaded shader [{}].", sBaseShaderName );
}

} // namespace Scion::Resources
