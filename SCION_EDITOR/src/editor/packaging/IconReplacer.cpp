#include "IconReplacer.h"
#include "Logger/Logger.h"

#ifdef _WIN32
#include <Windows.h>
// Ensure structures are packed correctly for icon format
#pragma pack( push, 2 )

/* Header of a .ico file. */
struct ICONDIR
{
	/* Reserved must be 0. */
	WORD reserved;
	/* Resource Type, 1 for icons. */
	WORD type;
	/* Number of images in the file. */
	WORD count;
};

/* Describes each icon image. */
struct ICONDIRENTRY
{
	/* Width of the icon. */
	BYTE width;
	/* Height of the icon. */
	BYTE height;
	/* Number of colors (0 if more than 256). */
	BYTE colorCount;
	/* Reserved (Must be 0). */
	BYTE reserved;
	/* Color planes. */
	BYTE planes;
	/* Bits per pixel. */
	WORD bitCount;
	/* Size of image data in bytes. */
	DWORD bytesInRes;
	/* Offset of image data from start of file. */
	DWORD imageOffset;
};

/* Header for RT_GROUP_ICON resource. */
struct GRPICONDIR
{
	/* Reserved, must be zero. */
	WORD reserved;
	/* Type, 1 for icons. */
	WORD type;
	/* Number of images. */
	WORD count;
};

/* Describes each icon in the resource group. */
struct GRPICONDIRENTRY
{
	/* Width of the icon. */
	BYTE width;
	/* Height of the icon. */
	BYTE height;
	/* Number of colors (0 if more than 256). */
	BYTE colorCount;
	/* Reserved (Must be 0). */
	BYTE reserved;
	/* Color planes. */
	WORD planes;
	/* Bits per pixel. */
	WORD bitCount;
	/* Size of image data in bytes. */
	DWORD bytesInRes;
	/* Resource ID for the icon image. */
	WORD id;
};
#pragma pack( pop ) // End of packing

#else
// TODO: add any linux includes as necessary.
#endif

namespace SCION_EDITOR
{

class IconReplacer::Impl
{
  public:
	Impl( const std::string& sIconFile, const std::string& sExeFile )
		: m_sIconFile{ sIconFile }
		, m_sExeFile{ sExeFile }
	{
	}

#ifdef _WIN32
	bool ReplaceIcon_Win();
#else
	bool ReplaceIcon_Linux();
#endif

  private:
	std::string m_sIconFile;
	std::string m_sExeFile;
};

IconReplacer::IconReplacer( const std::string& sIconFile, const std::string& sExeFile )
	: m_pImpl{ std::make_unique<Impl>( sIconFile, sExeFile ) }
{
}

IconReplacer::~IconReplacer() = default;

bool IconReplacer::ReplaceIcon()
{
#ifdef _WIN32
	return m_pImpl->ReplaceIcon_Win();
#else
	return m_pImpl->ReplaceIcon_Linux();
#endif
}

#ifdef _WIN32

bool IconReplacer::Impl::ReplaceIcon_Win()
{
	// Open the .ico file in binary mode
	std::ifstream file( m_sIconFile, std::ios::binary );
	if ( !file )
	{
		SCION_ERROR( "Failed to replace icon: Icon file could not be opened." );
		if ( file.fail() )
			SCION_ERROR( "Failed to replace icon: Stream failbit set (invalid format or operation)." );
		if ( file.bad() )
			SCION_ERROR(
				"Failed to replace icon: Stream badbit set (irrecoverable error, e.g.hardware I/O failure.)." );
		if ( !file.good() )
			SCION_ERROR( "Failed to replace icon: General stream is not good." );

		return false;
	}

	ICONDIR iconDir;
	file.read( reinterpret_cast<char*>( &iconDir ), sizeof( ICONDIR ) );

	// Only support one icon image for simplicity.
	if ( iconDir.count != 1 )
	{
		SCION_ERROR( "Failed to replace icon: Image count is not 1 for [{}].\n"
					 "Icons with multiple images are not supported.",
					 m_sIconFile );

		return false;
	}

	ICONDIRENTRY entry;
	file.read( reinterpret_cast<char*>( &entry ), sizeof( ICONDIRENTRY ) );
	// Allocate a buffer for the image data
	std::vector<char> image( entry.bytesInRes );
	// Move to the image offset
	file.seekg( entry.imageOffset, std::ios::beg );
	// Read the image data
	file.read( image.data(), entry.bytesInRes );

	GRPICONDIR grpDir{ 0, 1, 1 };

	GRPICONDIRENTRY grpEntry{ entry.width,
							  entry.height,
							  entry.colorCount,
							  entry.reserved,
							  entry.planes,
							  entry.bitCount,
							  entry.bytesInRes,
							  1 };

	HANDLE hUpdate = BeginUpdateResourceA( m_sExeFile.c_str(), FALSE );
	if ( !hUpdate )
	{
		SCION_ERROR( "Failed to replace icon: Unable to open update handle." );
		return false;
	}

	// Write the RT_ICON resource with ID 1
	if ( !UpdateResourceA( hUpdate, RT_ICON, MAKEINTRESOURCEA( 1 ), 0, image.data(), entry.bytesInRes ) )
	{
		EndUpdateResourceA( hUpdate, TRUE );
		SCION_ERROR( "Failed to update RT_ICON." );
		return false;
	}

	// Combine GRPICONDIR and GRPICONDIRENTRY into one memory block
	std::vector<char> groupResource( sizeof( GRPICONDIR ) + sizeof( GRPICONDIRENTRY ) );
	memcpy( groupResource.data(), &grpDir, sizeof( GRPICONDIR ) );
	memcpy( groupResource.data() + sizeof(GRPICONDIR), &grpEntry, sizeof( GRPICONDIRENTRY ) );

	// Write the RT_GROUP_ICON resource with ID 1
	if ( !UpdateResourceA( hUpdate, RT_GROUP_ICON, MAKEINTRESOURCEA( 1 ), 0, groupResource.data(), groupResource.size() ) )
	{
		EndUpdateResourceA( hUpdate, TRUE );
		SCION_ERROR( "Failed to update RT_GROUP_ICON." );
		return false;
	}

	// Commit the changes
	return EndUpdateResourceA( hUpdate, FALSE );
}

#else
bool IconReplacer::Impl::ReplaceIcon_Linux()
{
	// TODO: Handle Linux Implementation
	return false;
}
#endif

} // namespace SCION_EDITOR
