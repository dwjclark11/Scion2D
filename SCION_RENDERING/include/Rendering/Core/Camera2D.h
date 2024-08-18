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
	~Camera2D() = default;
	/*
	 * @brief Updates the camera's position, scale,
	 * and rotation if needed. If there has been no changes,
	 * no updates occur
	 */
	void Update();

	void Reset();
	void Resize( int width, int height );

	glm::vec2 ScreenCoordsToWorld( const glm::vec2& screenCoords ) const;
	glm::vec2 WorldCoordsToScreen( const glm::vec2& screenCoords ) const;

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
