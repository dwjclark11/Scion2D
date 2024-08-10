#pragma once
#include <glm/glm.hpp>

namespace SCION_RENDERING
{
class Camera2D;
}

namespace SCION_CORE::ECS
{
class Registry;
}

namespace SCION_EDITOR
{

struct Canvas;

class AbstractTool
{
  private:
	glm::vec2 m_MouseScreenCoords, m_MouseWorldCoords;
	glm::vec2 m_GUICursorCoords, m_GUIRelativeCoords;
	glm::vec2 m_WindowPos, m_WindowSize;

	bool m_bActivated, m_bOutOfBounds;

  private:
	void UpdateMouseWorldCoords();
	void CheckOutOfBounds( Canvas& canvas );

  protected:
	enum class EMouseButton
	{
		UNKNOWN = 0,
		LEFT,
		MIDDLE,
		RIGHT,

		LAST
	};

	SCION_CORE::ECS::Registry* m_pRegistry{ nullptr };
	SCION_RENDERING::Camera2D* m_pCamera{ nullptr };

  protected:
	bool MouseBtnJustPressed( EMouseButton eButton );
	bool MouseBtnJustReleased( EMouseButton eButton );
	bool MouseBtnPressed( EMouseButton eButton );
	bool MouseMoving();

	/*
	 * @brief Used for different tools to make adjustments to the mouse position if necessary.
	 */
	virtual void ExamineMousePosition() = 0;

	inline void SetMouseWorldCoords( const glm::vec2& newCoords ) { m_MouseWorldCoords = newCoords; }

  public:
	AbstractTool();
	virtual ~AbstractTool() = default;

	/*
	 * @brief Update the tool. If overrided, ensure to call the parent update. This will make sure the
	 * mouse world coords are updated accordingly.
	 */
	virtual void Update( Canvas& canvas );

	/*
	 * @brief Sets the current registry to the registry of the current scene and the camera.
	 */
	bool SetupTool( SCION_CORE::ECS::Registry* pRegistry, SCION_RENDERING::Camera2D* pCamera );

	inline void SetRelativeCoords( const glm::vec2& relativeCoords ) { m_GUIRelativeCoords = relativeCoords; }
	inline void SetCursorCoords( const glm::vec2& cursorCoords ) { m_GUICursorCoords = cursorCoords; }
	inline void SetWindowPos( const glm::vec2& windowPos ) { m_WindowPos = windowPos; }
	inline void SetWindowSize( const glm::vec2& windowSize ) { m_WindowSize = windowSize; }

	inline const glm::vec2& GetMouseScreenCoords() const { return m_MouseScreenCoords; }
	inline const glm::vec2& GetMouseWorldCoords() const { return m_MouseWorldCoords; }

	inline void Activate() { m_bActivated = true; }
	inline void Deactivate() { m_bActivated = false; }
	inline const bool IsActivated() const { return m_bActivated; }
	inline const bool OutOfBounds() const { return m_bOutOfBounds; }
};
} // namespace SCION_EDITOR