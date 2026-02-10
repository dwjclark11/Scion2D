#pragma once
#include "IDisplay.h"

namespace Scion::Core
{
struct GameConfig;
}

namespace Scion::Editor
{
class PackageGameDisplay : public IDisplay
{
  public:
	PackageGameDisplay();
	~PackageGameDisplay();

	virtual void Update() override;
	virtual void Draw() override;

private:
	bool CanPackageGame() const;

  private:
	std::unique_ptr<Scion::Core::GameConfig> m_pGameConfig;
	std::unique_ptr<class Packager> m_pPackager;
	std::string m_sDestinationPath;
	std::string m_sScriptListPath;
	std::string m_sFileIconPath;

	bool m_bResizable;
	bool m_bBorderless;
	bool m_bFullScreen;
	bool m_bTitlebar;

	bool m_bScriptListExists;
	bool m_bPackageHasErrors;
};
} // namespace Scion::Editor
