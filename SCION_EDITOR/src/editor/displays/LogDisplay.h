#pragma once
#include "IDisplay.h"
#include <imgui.h>

namespace SCION_EDITOR
{
class LogDisplay : public IDisplay
{
  private:
	ImGuiTextBuffer m_TextBuffer;
	ImVector<int> m_TextOffsets;
	bool m_bAutoScroll;

  private:
	void GetLogs();

	public:
	LogDisplay();
	~LogDisplay() = default;

	void Clear();
	virtual void Draw() override;
};
} // namespace SCION_EDITOR