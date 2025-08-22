#pragma once

namespace SCION_UTIL
{
enum class AssetType;
}

namespace SCION_EDITOR
{
	class AssetDisplayUtils
	{
	  public:
		AssetDisplayUtils() = delete;
		static std::vector<const char*> GetAssetFileFilters( SCION_UTIL::AssetType eAssetType );
		static std::string AddAssetBasedOnType( SCION_UTIL::AssetType eAssetType );
		static void OpenAddAssetModalBasedOnType( SCION_UTIL::AssetType eAssetType, bool* pbOpen );
	};
}
