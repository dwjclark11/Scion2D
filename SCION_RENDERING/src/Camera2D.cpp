#include "Rendering/Core/Camera2D.h"

namespace SCION_RENDERING {
	Camera2D::Camera2D()
		: Camera2D(640, 480)
	{
	}

	Camera2D::Camera2D(int width, int height)
		: m_Width{ width }, m_Height{ height }, m_Scale{ 1.f }
		, m_Position{ 0.f }, m_ScreenOffset{ 0.f }, m_CameraMatrix{ 1.f }, m_OrthoProjection{ 1.f }, m_bNeedsUpdate{ true }
	{
		// Init ortho projection
		m_OrthoProjection = glm::ortho(
			0.f,							// Left
			static_cast<float>(m_Width),	// Right
			static_cast<float>(m_Height),	// Top
			0.f,							// Bottom
			0.f,							// Near
			1.f								// Far
		);

		Update();
	}

	void Camera2D::Update()
	{
		if (!m_bNeedsUpdate)
			return;
		// Translate
		glm::vec3 translate{ -m_Position.x, -m_Position.y, 0.f };
		m_CameraMatrix = glm::translate(m_OrthoProjection, translate);

		// Scale
		glm::vec3 scale{ m_Scale, m_Scale, 0.f };
		m_CameraMatrix *= glm::scale(glm::mat4(1.f), scale);

		m_bNeedsUpdate = false;
	}

	glm::vec2 Camera2D::ScreenCoordsToWorld(const glm::vec2& screenCoords)
	{
		glm::vec2 worldCoords{screenCoords};

		// Set the coords to the center of the screen
		worldCoords -= m_ScreenOffset;

		// Scale the coordinates
		worldCoords /= m_Scale;

		// Translate the camera
		worldCoords += m_Position;
		
		return worldCoords;
	}

	glm::vec2 Camera2D::WorldCoordsToScreen(const glm::vec2& worldCoords)
	{
		glm::vec2 screenCoords{worldCoords};

		// Set the coords to the center of the screen
		screenCoords += m_ScreenOffset;

		// Scale the coordinates
		screenCoords *= m_Scale;

		// Translate the camera
		screenCoords -= m_Position;

		return screenCoords;
	}
}

