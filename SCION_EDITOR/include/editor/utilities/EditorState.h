#pragma once

namespace Scion::Core
{
class ProjectInfo;
}

namespace Scion::Editor
{

// clang-format off
enum class EDisplay : uint64_t
{
	None				= 0,
	Hierarchy			= 1 << 0,
	Tileset				= 1 << 1,
	TileDetails			= 1 << 2,
	AssetBrowser		= 1 << 3,
	SceneView			= 1 << 4,
	TilemapView			= 1 << 5,
	ContentBrowser		= 1 << 6,
	ScriptListView		= 1 << 7,
	PackagerView		= 1 << 8,
	GameSettingsView	= 1 << 9,
	Console				= 1 << 10,
	ObjectDetails		= 1 << 11,

	// TODO: add more displays
};

// clang-format on

struct EditorState
{
	uint64_t activeDisplays{
		static_cast<uint64_t>( EDisplay::Hierarchy ) | static_cast<uint64_t>( EDisplay::AssetBrowser ) |
		static_cast<uint64_t>( EDisplay::Console ) | static_cast<uint64_t>( EDisplay::TilemapView ) };

	bool bShowCollisions{ false };
	bool bShowAnimations{ false };
	bool bEnableGridsnap{ true };

	bool Save( Scion::Core::ProjectInfo& projectInfo );
	bool Load( Scion::Core::ProjectInfo& projectInfo );
	bool ImportState( const std::string& sImportedStateFile, Scion::Core::ProjectInfo& projectInfo );

	inline bool IsDisplayOpen( EDisplay display ) const
	{
		return ( activeDisplays & static_cast<uint64_t>( display ) ) != 0;
	}

	inline void SetDisplay( EDisplay display, bool bOpen )
	{
		if ( bOpen )
		{
			activeDisplays |= static_cast<uint64_t>( display );
		}
		else
		{
			activeDisplays &= ~static_cast<uint64_t>( display );
		}
	}

	inline void ToggleDisplay( EDisplay display ) { activeDisplays ^= static_cast<uint64_t>( display ); }

	static bool CreateEditorStateFile( Scion::Core::ProjectInfo& projectInfo );
};

using EditorStatePtr = std::shared_ptr<EditorState>;

} // namespace Scion::Editor
