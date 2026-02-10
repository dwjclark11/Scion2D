#pragma once
#include <rapidjson/document.h>

namespace Scion::Core
{
class ProjectInfo;
struct GameConfig;
} // namespace Scion::Core

namespace Scion::Utilities
{
class ThreadPool;
}

namespace Scion::Editor
{
struct PackageData
{
	std::unique_ptr<Scion::Core::ProjectInfo> pProjectInfo{ nullptr };
	std::unique_ptr<Scion::Core::GameConfig> pGameConfig{ nullptr };
	std::string sTempDataPath{};
	std::string sAssetFilepath{};
	std::string sFinalDestination{};
};

struct PackagingProgress
{
	float percent{ 0.f };
	std::string sMessage{};
};

class Packager
{
  public:
	Packager( std::unique_ptr<PackageData> pData, std::shared_ptr<Scion::Utilities::ThreadPool> pThreadPool );
	~Packager();

	bool Completed() const;
	bool HasError() const;

	PackagingProgress GetProgress() const;

	void FinishPackaging();

  private:
	void RunPackager();
	void UpdateProgress( float percent, std::string_view message );
	std::string CreateConfigFile( const std::string& sTempFilepath );
	std::string CreateAssetDefsFile( const std::string& sTempFilepath, const rapidjson::Value& assets );
	std::vector<std::string> CreateSceneFiles( const std::string& sTempFilepath, const rapidjson::Value& scenes );
	void CopyFilesToDestination();
	void CopyAssetsToDestination();

  private:
	std::unique_ptr<PackageData> m_pPackageData;
	std::thread m_PackageThread;
	std::atomic_bool m_bPackaging;
	std::atomic_bool m_bHasError;
	mutable std::shared_mutex m_ProgressMutex;
	PackagingProgress m_Progress;

	std::shared_ptr<Scion::Utilities::ThreadPool> m_pThreadPool;
};

} // namespace Scion::Editor
