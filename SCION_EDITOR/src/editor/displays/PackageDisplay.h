#pragma once
#include "IDisplay.h"
#include <memory>
#include <string>

namespace SCION_CORE
{
struct GameConfig;
}

namespace SCION_EDITOR
{
class PackageGameDisplay : public IDisplay
{
  public:
	PackageGameDisplay();
	~PackageGameDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	std::unique_ptr<SCION_CORE::GameConfig> m_pGameConfig;
	std::string m_sDestinationPath;
	std::string m_sScriptListPath;
	std::string m_sFileIconPath;

	bool m_bResizable;
	bool m_bBorderless;
	bool m_bFullScreen;
	bool m_bTitlebar;

	bool m_bScriptListExists;
};
} // namespace SCION_EDITOR
