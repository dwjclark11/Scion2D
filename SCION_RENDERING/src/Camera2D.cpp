#include "Rendering/Core/Camera2D.h"

namespace SCION_RENDERING
{
void Camera2D::Initialize()
{
	float width = static_cast<float>( m_Width );
	float height = static_cast<float>( m_Height );
	// Init ortho projection
	m_OrthoProjection = glm::ortho( 0.f, width, height, 0.f, 0.f, 1.f );

	Update();
}

Camera2D::Camera2D()
	: Camera2D( 640, 480 )
{
}

Camera2D::Camera2D( int width, int height )
	: m_Width{ width }
	, m_Height{ height }
	, m_Scale{ 1.f }
	, m_Position{ 0.f }
	, m_ScreenOffset{ 0.f }
	, m_CameraMatrix{ 1.f }
	, m_OrthoProjection{ 1.f }
	, m_bNeedsUpdate{ true }
{
	Initialize();
}

void Camera2D::Update()
{
	if ( !m_bNeedsUpdate )
		return;
	// Translate
	glm::vec3 translate{ -m_Position.x + m_ScreenOffset.x, -m_Position.y + m_ScreenOffset.y, 0.f };
	m_CameraMatrix = glm::translate( m_OrthoProjection, translate );

	// Scale
	glm::vec3 scale{ m_Scale, m_Scale, 0.f };
	m_CameraMatrix *= glm::scale( glm::mat4( 1.f ), scale );

	m_bNeedsUpdate = false;
}

void Camera2D::Reset()
{
	m_Scale = 1.f;
	m_Position = glm::vec2{ 0.f };
	m_ScreenOffset = glm::vec2{ 0.f };
	m_bNeedsUpdate = true;
}

void Camera2D::Resize( int width, int height )
{
	m_Width = width;
	m_Height = height;

	Initialize();
}

glm::vec2 Camera2D::ScreenCoordsToWorld( const glm::vec2& screenCoords ) const
{
	glm::vec2 worldCoords{ screenCoords };

	// Set the coords to the center of the screen
	worldCoords -= m_ScreenOffset;

	// Scale the coordinates
	worldCoords /= m_Scale;

	// Translate the camera
	worldCoords += m_Position;

	return worldCoords;
}

glm::vec2 Camera2D::WorldCoordsToScreen( const glm::vec2& worldCoords ) const
{
	glm::vec2 screenCoords{ worldCoords };

	// Set the coords to the center of the screen
	screenCoords += m_ScreenOffset;

	// Scale the coordinates
	screenCoords *= m_Scale;

	// Translate the camera
	screenCoords -= m_Position;

	return screenCoords;
}
} // namespace SCION_RENDERING
