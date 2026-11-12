#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

#include "Core/CoreUtilities/FollowCamera.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/ECS/Components/SpriteComponent.h"
#include <Logger/Logger.h>

using namespace Scion::Core::ECS;

namespace Scion::Core
{

FollowCamera::FollowCamera( Scion::Rendering::Camera2D& camera, const ECS::Entity& entity,
							const FollowCamParams& params )
	: m_Camera{ camera }
	, m_Params{ params }
	, m_Entity{ entity }
	, bEnabled{ true }
{
	SCION_ASSERT( m_Params.scale > 0.f && "The scale must be greater than zero!" );

	m_Params.springback = std::clamp( m_Params.springback, 0.f, 1.f );
	camera.SetScale( m_Params.scale );
}

void FollowCamera::Update()
{
	if ( !bEnabled )
		return;

	const auto& transform = m_Entity.GetComponent<TransformComponent>();

	float halfSpriteW{ 0.f };
	float halfSpriteH{ 0.f };
	
	if ( auto* pSprite = m_Entity.TryGetComponent<SpriteComponent>() )
	{
		halfSpriteW = pSprite->width * transform.scale.x * 0.5f;
		halfSpriteH = pSprite->height * transform.scale.y * 0.5f;
	}

	float camScale = m_Params.scale;
	if ( m_Camera.GetScale() != camScale )
	{
		m_Camera.SetScale( camScale );
	}

	float scaledCamWidth = m_Camera.GetWidth() / camScale;
	float scaledCamHeight = m_Camera.GetHeight() / camScale;

	glm::vec2 camPos = m_Camera.GetPosition();
	glm::vec2 newCamPosition{ 0.f };

	// Convert screen size to WORLD space based on zoom
	float halfW = scaledCamWidth * 0.5f;
	float halfH = scaledCamHeight * 0.5f;

	// Center camera on entity in WORLD coordinates
	newCamPosition.x = ( transform.position.x * transform.scale.x + halfSpriteW ) - halfW;
	newCamPosition.y = ( transform.position.y * transform.scale.y + halfSpriteH ) - halfH;

	float minX = m_Params.minX;
	float maxX = m_Params.maxX - scaledCamWidth;

	if ( minX > maxX )
	{
		std::swap( minX, maxX );
	}

	// Clamp camera top-left in WORLD space
	newCamPosition.x = std::clamp( newCamPosition.x, minX, maxX );

	float minY = m_Params.minY;
	float maxY = m_Params.maxY - scaledCamHeight;

	if ( minY > maxY )
	{
		std::swap( minY, maxY );
	}
	newCamPosition.y = std::clamp( newCamPosition.y, minY, maxY );

	// Smooth camera movement
	glm::vec2 lerped{ std::lerp( camPos.x, newCamPosition.x, m_Params.springback ),
					  std::lerp( camPos.y, newCamPosition.y, m_Params.springback ) };
	if ( m_bSnap )
	{
		lerped = newCamPosition;
		m_bSnap = false;
	}

	m_Camera.SetPosition( lerped );
	// SCION_LOG( "Cam: x = {}, y = {}", lerped.x, lerped.y );
	// SCION_LOG( "Min: x = {}, y = {}", minX, minY);
	// SCION_LOG( "Transform: x = {}, y = {}", transform.position.x, transform.position.y );
}

void FollowCamera::SetSpringback( float springback )
{
	m_Params.springback = std::clamp( springback, 0.f, 1.f );
}

void FollowCamera::SetCameraParameters( const FollowCamParams& params )
{
	m_Params = params;

	if ( m_Params.scale <= 0.f )
		m_Params.scale = 0.1f;

	if ( m_Params.springback < 0.f )
		m_Params.springback = 0.f;
}

void FollowCamera::CreateLuaFollowCamera( sol::state& lua, ECS::Registry& registry )
{
	// clang-format off
	lua.new_usertype<FollowCamParams>(
		"FollowCamParams",
		sol::call_constructor,
		sol::factories(
			[]( const sol::table& params ) {
				return FollowCamParams{
					.minX = params[ "minX" ].get_or( 0.f ),
					.minY = params[ "minY" ].get_or( 0.f ),
					.scale = params[ "scale" ].get_or( 1.f ),
					.springback = params[ "springback" ].get_or( 1.f ),
					.maxX = params[ "maxX" ].get_or( static_cast<float>( CoreEngineData::GetInstance().WindowWidth() ) ),
					.maxY = params[ "maxY" ].get_or( static_cast<float>( CoreEngineData::GetInstance().WindowHeight() ) )
				};
			},
			[]( float minX, float minY, float maxX, float maxY, float scale, float springback ) {
				return FollowCamParams{
					.minX = minX, .minY = minY, .scale = scale, .springback = springback, .maxX = maxX, .maxY = maxY };
			} ),
		"minX", &FollowCamParams::minX,
		"minY", &FollowCamParams::minY,
		"maxX", &FollowCamParams::maxX,
		"maxY", &FollowCamParams::maxY,
		"scale", &FollowCamParams::scale,
		"springback", &FollowCamParams::springback );

	auto& camera = registry.GetContext<std::unique_ptr<Scion::Rendering::Camera2D>>();

	lua.new_usertype<FollowCamera>(
		"FollowCamera",
		sol::call_constructor,
		sol::factories(
			[ & ]( const FollowCamParams& params, const ECS::Entity& entity )
			{
				return FollowCamera{ *camera, entity, params };
			}
		),
		"update", &FollowCamera::Update,
		"setMinX", [](FollowCamera& followCam, float minX) {
			followCam.m_Params.minX = minX;
		},
		"setMaxX", [](FollowCamera& followCam, float maxX) {
			followCam.m_Params.maxX = maxX;
		},
		"setMinY", [](FollowCamera& followCam, float minY) {
			followCam.m_Params.minY = minY;
		},
		"setMaxY", [](FollowCamera& followCam, float maxY) {
			followCam.m_Params.maxY = maxY;
		},
		"setParams", &FollowCamera::SetCameraParameters,
		"setEntity", &FollowCamera::SetEntity,
		"setSpringback", &FollowCamera::SetSpringback,
		"getParams", &FollowCamera::GetParams,
		"bEnabled", &FollowCamera::bEnabled
	);
	// clang-format on
}

} // namespace Scion::Core
