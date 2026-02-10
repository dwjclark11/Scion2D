#pragma once
#include "IDisplay.h"

namespace Scion::Core
{
class ProjectInfo;
class CoreEngineData;
namespace ECS
{
class MainRegistry;
}
} // namespace Scion::Core

namespace Scion::Editor
{
class ProjectSettingsDisplay : public IDisplay
{
  public:
	ProjectSettingsDisplay();
	~ProjectSettingsDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	struct SettingItem
	{
		std::string sName{};
		std::function<void()> drawFunc;
	};

	struct SettingCategory
	{
		std::string sName;
		std::vector<SettingItem> items;
		std::vector<SettingCategory> subCategories;
	};

	void DrawCategoryTree( const SettingCategory& category, std::string& selected );
	void DrawSettingsPanel( const SettingCategory& category, std::string& selected );
	void CreateProjectSettings();

	SettingCategory CreateGeneralSettings( Scion::Core::CoreEngineData& coreGlobals,
										   Scion::Core::ProjectInfo& projectInfo,
										   Scion::Core::ECS::MainRegistry& mainRegistry );

	SettingCategory CreatePhysicsSettings( Scion::Core::CoreEngineData& coreGlobals,
										   Scion::Core::ProjectInfo& projectInfo,
										   Scion::Core::ECS::MainRegistry& mainRegistry );

	SettingCategory CreateGraphicsSettings( Scion::Core::CoreEngineData& coreGlobals,
											Scion::Core::ProjectInfo& projectInfo,
											Scion::Core::ECS::MainRegistry& mainRegistry );

	SettingCategory CreateAudioSettings( Scion::Core::CoreEngineData& coreGlobals, Scion::Core::ProjectInfo& projectInfo,
										 Scion::Core::ECS::MainRegistry& mainRegistry );

  private:
	SettingCategory m_SettingsCategory;
	std::string m_sSelectedCategory;
};
} // namespace Scion::Editor
