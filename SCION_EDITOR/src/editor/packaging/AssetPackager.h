#pragma once
#include <rapidjson/document.h>
#include <string>
#include <filesystem>

namespace SCION_UTIL
{
enum class AssetType;
class ThreadPool;
} // namespace SCION_UTIL

namespace SCION_FILESYSTEM
{
class LuaSerializer;
}

namespace SCION_EDITOR
{
struct AssetPackagerParams
{
	std::string sAssetsPath{};
	std::string sTempFilepath{};
	std::string sDestinationPath{};
	std::string sProjectPath{};
};

class AssetPackager
{
  public:
	AssetPackager( const AssetPackagerParams& params, std::shared_ptr<SCION_UTIL::ThreadPool> pThreadPool );
	~AssetPackager();

	void PackageAssets( const rapidjson::Value& assets );

  private:
	void ConvertAssetToLuaTable( SCION_FILESYSTEM::LuaSerializer& luaSerializer, const std::string& sAssetNum,
								 const std::string& sInAssetFile, SCION_UTIL::AssetType eType );

	void CreateLuaAssetFiles( const std::string& sProjectPath, const rapidjson::Value& assets );
	bool CompileLuaAssetFiles();
	bool CreateAssetsZip();

	struct AssetPackageStatus
	{
		std::string sError{};
		bool bSuccess{ false };
	};

	AssetPackageStatus SerializeAssetsByType( const rapidjson::Value& assets,
											  const std::filesystem::path& tempAssetsPath,
											  const std::string& sAssetTypeName, const std::string& sContentPath,
											  SCION_UTIL::AssetType eAssetType );

  private:
	AssetPackagerParams m_Params;
	std::shared_ptr<SCION_UTIL::ThreadPool> m_pThreadPool;
};

} // namespace SCION_EDITOR
