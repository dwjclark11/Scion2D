#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace SCION_RENDERING
{
class Camera2D
{
  private:
	int m_Width, m_Height;
	float m_Scale;

	glm::vec2 m_Position, m_ScreenOffset;
	glm::mat4 m_CameraMatrix, m_OrthoProjection;

	bool m_bNeedsUpdate;

  private:
	void Initialize();

  public:
	Camera2D();
	Camera2D( int width, int height );
	~Camera2D();
	/*
	 * @brief Updates the camera's position, scale,
	 * and rotation if needed. If there has been no changes,
	 * no updates occur
	 */
	void Update();

	/*
	 * @brief Resets all camera parameters back to default values.
	 */
	void Reset();

	/*
	 * @brief Resizes the width and the height of the camera and
	 * reinitializes the camera matrix.
	 */
	void Resize( int newWidth, int newHeight );

	/*
	 * @brief Converts the passed in screen coordinates to world coordinates.
	 */
	glm::vec2 ScreenCoordsToWorld( const glm::vec2& screenCoords ) const;

	/*
	 * @brief Converts the passed in world coordinates to screen coordinates.
	 */
	glm::vec2 WorldCoordsToScreen( const glm::vec2& worldCoords ) const;

	inline void SetPosition( glm::vec2 newPosition )
	{
		m_Position = newPosition;
		m_bNeedsUpdate = true;
	}

	inline void SetScreenOffset( glm::vec2 newOffset )
	{
		m_ScreenOffset = newOffset;
		m_bNeedsUpdate = true;
	}

	inline void SetScale( float scale )
	{
		m_Scale = scale;
		m_bNeedsUpdate = true;
	}

	inline const glm::vec2 GetPosition() const { return m_Position; }
	inline const glm::vec2 GetScreenOffset() const { return m_ScreenOffset; }

	inline const float GetScale() const { return m_Scale; }
	inline const int GetWidth() const { return m_Width; }
	inline const int GetHeight() const { return m_Height; }
	/*
	 * @brief Get the camera projection based on the orthographic projection matrix.
	 * @return Returns the camera matrix glm::mat4
	 */
	inline glm::mat4 GetCameraMatrix() const { return m_CameraMatrix; }
};
} // namespace SCION_RENDERING
