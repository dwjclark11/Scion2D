#pragma once
#include "IDisplay.h"

namespace SCION_CORE
{
class ProjectInfo;
class CoreEngineData;
namespace ECS
{
class MainRegistry;
}
} // namespace SCION_CORE

namespace SCION_EDITOR
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

	SettingCategory CreateGeneralSettings( SCION_CORE::CoreEngineData& coreGlobals,
										   SCION_CORE::ProjectInfo& projectInfo,
										   SCION_CORE::ECS::MainRegistry& mainRegistry );

	SettingCategory CreatePhysicsSettings( SCION_CORE::CoreEngineData& coreGlobals,
										   SCION_CORE::ProjectInfo& projectInfo,
										   SCION_CORE::ECS::MainRegistry& mainRegistry );

	SettingCategory CreateGraphicsSettings( SCION_CORE::CoreEngineData& coreGlobals,
											SCION_CORE::ProjectInfo& projectInfo,
											SCION_CORE::ECS::MainRegistry& mainRegistry );

	SettingCategory CreateAudioSettings( SCION_CORE::CoreEngineData& coreGlobals, SCION_CORE::ProjectInfo& projectInfo,
										 SCION_CORE::ECS::MainRegistry& mainRegistry );

  private:
	SettingCategory m_SettingsCategory;
	std::string m_sSelectedCategory;
};
} // namespace SCION_EDITOR
