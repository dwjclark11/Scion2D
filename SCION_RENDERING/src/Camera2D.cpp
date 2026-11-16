#include "Rendering/Core/Camera2D.h"
#include <sol/sol.hpp>
#include <glm/gtc/matrix_transform.hpp>

static float RandomFloat( float min, float max )
{
	static std::mt19937 rng{ std::random_device{}() };
	std::uniform_real_distribution<float> dist( min, max );
	return dist( rng );
}

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

void Camera2D::UpdateShake()
{
	if ( !m_bShakeActive )
		return;

	// Reduce shake over time
	// m_ShakeTimer -= dt;
	m_ShakeTimer -= 0.016f; // TODO: Send real DT in
	if ( m_ShakeTimer <= 0.0f )
	{
		m_bShakeActive = false;
		m_ShakeOffset = { 0.0f, 0.0f };
		m_ShakeRotation = 0.0f;
		m_bNeedsUpdate = true;
		return;
	}

	// Damped intensity
	float t = m_ShakeTimer / m_ShakeDuration;
	float intensity = m_ShakeAmount * t;

	// High-frequency jitter
	float rx = ( RandomFloat( -1.0f, 1.0f ) * intensity );
	float ry = ( RandomFloat( -1.0f, 1.0f ) * intensity );

	// Rotational jitter (very small values feel best)
	float rrot = RandomFloat( -1.0f, 1.0f ) * intensity * m_ShakeRotAmount;

	m_ShakeOffset.x = rx;
	m_ShakeOffset.y = ry;
	m_ShakeRotation = rrot;

	// Must update camera matrix
	m_bNeedsUpdate = true;
}

void Camera2D::RecalculateMatrix()
{
	// Calculate scale vector
	glm::vec3 scale{ m_Scale, m_Scale, 0.f };
	// Calculate the rotation
	float totalRotation = m_Rotation + m_ShakeRotation;
	// Calculate translation vector
	glm::vec3 translate{
		-m_Position.x + m_ScreenOffset.x + m_ShakeOffset.x, -m_Position.y + m_ScreenOffset.y + m_ShakeOffset.y, 0.f };

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
	, m_Rotation{ 0.f }
	, m_ShakeOffset{ 0.f }
	, m_ShakeRotation{ 0.f }
	, m_ShakeAmount{ 0.f }
	, m_ShakeRotAmount{ 0.f }
	, m_ShakeDuration{ 0.f }
	, m_ShakeTimer{ 0.f }
	, m_bShakeActive{ false }
{
	Initialize();
}

void Camera2D::Update()
{
	if ( m_bShakeActive )
		UpdateShake();

	if ( m_bNeedsUpdate )
		RecalculateMatrix();
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

void Camera2D::Shake( float intensity, float rotIntensity, float duration )
{
	if ( m_bShakeActive )
		return;

	m_ShakeAmount = intensity;
	m_ShakeRotAmount = rotIntensity;
	m_ShakeDuration = duration;
	m_ShakeTimer = duration;

	m_ShakeOffset = { 0.0f, 0.0f };
	m_ShakeRotation = 0.0f;

	m_bShakeActive = true;
	m_bNeedsUpdate = true;
}


void Camera2D::CreateLuaBind(sol::state& lua, Camera2D& camera)
{
	// clang-format off
	lua.new_usertype<Camera2D>(
		"Camera",
		sol::no_constructor,
		"get", [ &camera ]( sol::this_state s )
		{
			return sol::make_reference( s, std::ref( camera ) );
		},
		"position", [ &camera ] { return camera.GetPosition(); },
		"setPosition", sol::overload(
			[ &camera ]( const glm::vec2& newPosition ) { camera.SetPosition( newPosition ); },
			[ &camera ]( float x, float y ) { camera.SetPosition( x, y ); }
		),
		"scale", [ &camera ] { return camera.GetScale(); },
		"setScale", [ &camera ]( float scale )
		{
			camera.SetScale( scale );
		},
		"getWorldCoords", [ &camera ]( const glm::vec2& screenCoords ) { return camera.ScreenCoordsToWorld( screenCoords ); },
		"getScreenCoords", [ &camera ]( const glm::vec2& worldCoords ) { return camera.WorldCoordsToScreen( worldCoords ); },
		"width", [ &camera ] { return camera.GetWidth(); },
		"height", [ &camera ] { return camera.GetHeight(); },
		"rotation", [ &camera ] { return camera.GetRotation(); },
		"setRotation", [ &camera ]( float rotation ) { camera.SetRotation( rotation ); },
		"shake",
		[ &camera ]( float intensity, float rotIntensity, float duration ) {
			camera.Shake( intensity, rotIntensity, duration );
		},
		"move", sol::overload(
			[ &camera ]( const glm::vec2& distance ) { camera.SetPosition( camera.GetPosition() + distance); },
			[ &camera ]( float x, float y ) { camera.SetPosition( camera.GetPosition() + glm::vec2{x, y} ); }
		)
	);
	// clang-format off
}

} // namespace Scion::Rendering
