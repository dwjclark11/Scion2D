#pragma once
#include "IDisplay.h"
#include <imgui.h>
#include <string>

namespace SCION_EDITOR
{
class LogDisplay : public IDisplay
{
  public:
	LogDisplay();
	~LogDisplay() = default;

	void Clear();
	virtual void Draw() override;

  private:
	void GetLogs();

  private:
	ImGuiTextBuffer m_TextBuffer;
	ImVector<int> m_TextOffsets;
	std::string m_sSearchQuery;
	bool m_bAutoScroll;
	bool m_bShowInfo;
	bool m_bShowWarn;
	bool m_bShowError;
	int m_LogIndex;
};
} // namespace SCION_EDITOR
