#pragma once

namespace Scion::Utilities
{
enum class AssetType;
}

namespace Scion::Editor
{
	class AssetDisplayUtils
	{
	  public:
		AssetDisplayUtils() = delete;
		static std::vector<const char*> GetAssetFileFilters( Scion::Utilities::AssetType eAssetType );
		static std::string AddAssetBasedOnType( Scion::Utilities::AssetType eAssetType );
		static void OpenAddAssetModalBasedOnType( Scion::Utilities::AssetType eAssetType, bool* pbOpen );
	};
}
