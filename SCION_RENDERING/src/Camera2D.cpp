#include "Rendering/Core/Camera2D.h"

namespace Scion::Rendering
{
void Camera2D::Initialize()
{
	// Init ortho projection
	m_OrthoProjection =
		glm::ortho( 0.f, static_cast<float>( m_Width ), static_cast<float>( m_Height ), 0.f, -1.f, 1.f );
	m_bNeedsUpdate = true;
	Update();
}

Camera2D::Camera2D()
	: Camera2D( 640, 480 )
{
}

Camera2D::~Camera2D()
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

	// Calculate scale vector
	glm::vec3 scale{ m_Scale, m_Scale, 0.f };
	// Calculate the rotation
	float totalRotation = 0.f; // TODO: Add camera rotation
	// Calculate translation vector
	glm::vec3 translate{
		-m_Position.x + m_ScreenOffset.x, -m_Position.y + m_ScreenOffset.y, 0.f };

	// Correct order: apply scale first, then translate (right to left)
	// This is equivalent to: m_OrthoProjection * T * S * V
	glm::mat4 cameraView = glm::scale( glm::mat4( 1.f ), scale ); // Start with identity and apply scale
	// Apply the rotation to the scaled matrix
	cameraView = glm::rotate( cameraView, totalRotation, glm::vec3{ 0.f, 0.f, 1.f } );
	// Apply translation to the scaled matrix
	cameraView = glm::translate( cameraView, translate );
	// Combine with the projection matrix
	m_CameraMatrix = m_OrthoProjection * cameraView;
	// Finish the update
	m_bNeedsUpdate = false;
}

void Camera2D::Reset()
{
	m_Scale = 1.f;
	m_Position = glm::vec2{ 0.f };
	m_ScreenOffset = glm::vec2{ 0.f };
	m_bNeedsUpdate = true;
}

void Camera2D::Resize( int newWidth, int newHeight )
{
	m_Width = newWidth;
	m_Height = newHeight;

	Initialize();
}

glm::vec2 Camera2D::ScreenCoordsToWorld( const glm::vec2& screenCoords ) const
{
	glm::vec2 world = screenCoords;

	// Undo scale
	world /= m_Scale;
	// Undo screen offset
	world += -m_ScreenOffset;
	// Add camera position
	world += m_Position;

	return world;
}

glm::vec2 Camera2D::WorldCoordsToScreen( const glm::vec2& worldCoords ) const
{
	glm::vec2 screen = ( worldCoords - m_Position + m_ScreenOffset );
	screen *= m_Scale;
	return screen;
}
} // namespace Scion::Rendering
