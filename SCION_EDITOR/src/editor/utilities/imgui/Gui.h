#pragma once
#include <SDL.h>

namespace SCION_WINDOWING
{
class Window;
}

namespace SCION_EDITOR
{
class Gui
{
  public:
	Gui() = delete;

	static bool InitImGui( SCION_WINDOWING::Window* pWindow );
	static void Begin();
	static void End( SCION_WINDOWING::Window* pWindow );
	static void ShowImGuiDemo();

  private:
	inline static bool m_bInitialized{ false };
};
} // namespace SCION_EDITOR
