#pragma once
#include "IDisplay.h"

namespace SCION_EDITOR
{

class EditorStyleToolDisplay : public IDisplay
{
  private:
	void DrawSizePicker();
	void DrawColorPicker();

  public:
	EditorStyleToolDisplay();
	virtual ~EditorStyleToolDisplay() = default;

	virtual void Draw() override;
};
} // namespace SCION_EDITOR
