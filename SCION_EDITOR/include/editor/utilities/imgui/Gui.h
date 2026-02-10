#pragma once
#include <SDL.h>

namespace Scion::Windowing
{
class Window;
}

namespace Scion::Editor
{
class Gui
{
  public:
	Gui() = delete;

	/**
	 * @brief Initializes the ImGui context, fonts, and platform/render backends.
	 *
	 * Sets up the ImGui context and configuration flags, loads default and custom fonts,
	 * and initializes the SDL2 and OpenGL3 backends for rendering within the application window.
	 *
	 * @param pWindow Pointer to the Scion::Windowing::Window containing the SDL window and OpenGL context.
	 * @return true if initialization was successful, false if already initialized or if any step fails.
	 */
	static bool InitImGui( Scion::Windowing::Window* pWindow );

	/**
	 * @brief Begins a new ImGui frame.
	 *
	 * Prepares ImGui for a new frame by invoking backend-specific new frame functions
	 * and creating a new UI frame.
	 */
	static void Begin();

	/**
	 * @brief Ends the current ImGui frame and renders draw data.
	 *
	 * Finalizes ImGui rendering, dispatches draw data to the OpenGL backend, and handles
	 * multi-viewport rendering if enabled.
	 *
	 * @param pWindow Pointer to the Scion::Windowing::Window, used for viewport context restoration.
	 */
	static void End( Scion::Windowing::Window* pWindow );

	/**
	 * @brief Displays the built-in ImGui demo window.
	 *
	 * Useful for testing and previewing ImGui features and widgets.
	 */
	static void ShowImGuiDemo();

  private:
	inline static bool m_bInitialized{ false };
};
} // namespace Scion::Editor
