#pragma once
#include "IDisplay.h"

namespace SCION_EDITOR
{

class EditorStyleToolDisplay : public IDisplay
{
  public:
	EditorStyleToolDisplay();
	virtual ~EditorStyleToolDisplay() = default;

	virtual void Draw() override;

private:
	void DrawSizePicker();
	void DrawColorPicker();
};
} // namespace SCION_EDITOR
