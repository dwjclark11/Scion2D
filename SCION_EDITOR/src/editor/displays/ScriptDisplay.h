#pragma once
#include "IDisplay.h"

namespace SCION_FILESYSTEM
{
class DirectoryWatcher;
}

namespace SCION_EDITOR
{
class ScriptDisplay : public IDisplay
{
  public:
	ScriptDisplay();
	virtual ~ScriptDisplay();

	virtual void Draw() override;
	virtual void Update() override;

  private:
	void GenerateScriptList();
	void WriteScriptListToFile();
	void OnFileChanged( const std::filesystem::path& path, bool bModified );

  private:
	std::vector<std::string> m_ScriptList;
	std::vector<std::string> m_Scripts;
	std::string m_sScriptsDirectory;
	int m_Selected;
	bool m_bScriptsChanged;
	bool m_bListScripts;

	std::unique_ptr<SCION_FILESYSTEM::DirectoryWatcher> m_pDirWatcher;
	std::atomic_bool m_bFilesChanged;
};

} // namespace SCION_EDITOR
