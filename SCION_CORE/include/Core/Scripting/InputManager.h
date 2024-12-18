#pragma once
#include <Windowing/Inputs/Keyboard.h>
#include <Windowing/Inputs/Mouse.h>
#include <Windowing/Inputs/Gamepad.h>
#include <memory>
#include <sol/sol.hpp>
#include "Core/ECS/Registry.h"

using namespace SCION_WINDOWING::Inputs;
constexpr int MAX_CONTROLLERS = 4;

#define INPUT_MANAGER() SCION_CORE::InputManager::GetInstance()

namespace SCION_CORE
{
/*
 * @brief InputManager class controls all necessary inputs for the game.
 * @brief Contains the Keyboard, the Mouse, and up to 4 Gamepads/Controllers
 */
class InputManager
{
  private:
	std::unique_ptr<Keyboard> m_pKeyboard;
	std::unique_ptr<Mouse> m_pMouse;
	std::map<int, std::shared_ptr<Gamepad>> m_mapGameControllers;

  private:
	InputManager();
	~InputManager() = default;
	InputManager( const InputManager& ) = delete;
	InputManager& operator=( const InputManager& ) = delete;

  private:
	static void RegisterLuaKeyNames( sol::state& lua );
	static void RegisterMouseBtnNames( sol::state& lua );
	static void RegisterGamepadBtnNames( sol::state& lua );

  public:
	static InputManager& GetInstance();
	static void CreateLuaInputBindings( sol::state& lua, SCION_CORE::ECS::Registry& registry );

	inline Keyboard& GetKeyboard() { return *m_pKeyboard; }
	inline Mouse& GetMouse() { return *m_pMouse; }

	inline std::map<int, std::shared_ptr<Gamepad>>& GetControllers() { return m_mapGameControllers; }

	/*
	 * @brief Get the controller at a desired index.
	 * @param Takes an int for the index of the controller.
	 * @return Returns a shared_ptr of the gamepad at the index
	 * if exists, else returns nullptr;
	 */
	std::shared_ptr<Gamepad> GetController( int index );

	/*
	 * @brief Adds a new gamepad to the map at the given index.
	 * @param Takes in a Sint32 for the desired index.
	 * @return Returns true if successfully added, false otherwise.
	 */
	bool AddGamepad( Sint32 gamepadIndex );

	/*
	 * @brief Removes the gamepad based on the given ID
	 * @param Takes in a Sin32 for the gamepadID
	 * @return Returns true if the gamepad was removed successfully, false otherwise.
	 */
	bool RemoveGamepad( Sint32 gamepadID );

	/*
	 * @brief Finds the Gamepad based on the values provided from the SDL_Event
	 * and sets the Button to pressed if it exists.
	 * @param Takes in the SDL_Event
	 */
	void GamepadBtnPressed( const SDL_Event& event );

	/*
	 * @brief Finds the Gamepad based on the values provided from the SDL_Event
	 * and sets the Button to released if it exists.
	 * @param Takes in the SDL_Event
	 */
	void GamepadBtnReleased( const SDL_Event& event );

	/*
	 * @brief Finds the Gamepad based on the values provided from the SDL_Event
	 * and sets the axis values.
	 * @brief Access values are treated as analog values that have a range.
	 * @param Takes in the SDL_Event
	 */
	void GamepadAxisValues( const SDL_Event& event );

	/*
	 * @brief Finds the Gamepad based on the values provided from the SDL_Event
	 * and sets the Hat values if they exists.
	 * @param Takes in the SDL_Event
	 */
	void GamepadHatValues( const SDL_Event& event );

	/*
	 * @brief Updates all active gamepads
	 */
	void UpdateGamepads();
};
} // namespace SCION_CORE
