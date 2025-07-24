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

	/**
	 * @brief Initializes the ImGui context, fonts, and platform/render backends.
	 *
	 * Sets up the ImGui context and configuration flags, loads default and custom fonts,
	 * and initializes the SDL2 and OpenGL3 backends for rendering within the application window.
	 *
	 * @param pWindow Pointer to the SCION_WINDOWING::Window containing the SDL window and OpenGL context.
	 * @return true if initialization was successful, false if already initialized or if any step fails.
	 */
	static bool InitImGui( SCION_WINDOWING::Window* pWindow );

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
	 * @param pWindow Pointer to the SCION_WINDOWING::Window, used for viewport context restoration.
	 */
	static void End( SCION_WINDOWING::Window* pWindow );

	/**
	 * @brief Displays the built-in ImGui demo window.
	 *
	 * Useful for testing and previewing ImGui features and widgets.
	 */
	static void ShowImGuiDemo();

  private:
	inline static bool m_bInitialized{ false };
};
} // namespace SCION_EDITOR
