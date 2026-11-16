#include "Core/Systems/RenderShapeSystem.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/Components/BoxColliderComponent.h"
#include "Core/ECS/Components/CircleColliderComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/ECS/Components/PhysicsComponent.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/CoreUtilities.h"

#include <Rendering/Core/Camera2D.h>
#include <Rendering/Essentials/Primitives.h>
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Core/ShapeBatchRenderer.h>

#include "ScionUtilities/MathUtilities.h"

using namespace Scion::Core::ECS;
using namespace Scion::Rendering;
using namespace Scion::Resources;

namespace Scion::Core::Systems
{

namespace
{
constexpr int LAYER_BOX = 0;
constexpr int LAYER_CIRCLE = 1;
} // namespace

RenderShapeSystem::RenderShapeSystem()
	: m_pShapeRenderer{ std::make_unique<ShapeRenderer>() }
{
}

RenderShapeSystem::~RenderShapeSystem() = default;

void RenderShapeSystem::Update( Scion::Core::ECS::Registry& registry, Scion::Rendering::Camera2D& camera )
{
	auto& assetManager = MAIN_REGISTRY().GetAssetManager();

	auto colorShader = assetManager.GetShader( "color" );
	colorShader->Enable();
	colorShader->SetUniformMat4( "uProjection", camera.GetCameraMatrix() );

	m_pShapeRenderer->Begin();

	auto boxView = registry.GetRegistry().view<TransformComponent, BoxColliderComponent>();
	for ( auto entity : boxView )
	{
		const auto& transform = boxView.get<TransformComponent>( entity );
		const auto& boxCollider = boxView.get<BoxColliderComponent>( entity );

		if ( !Scion::Core::EntityInView( transform,
										 static_cast<float>( boxCollider.width ),
										 static_cast<float>( boxCollider.height ),
										 camera ) )
		{
			continue;
		}
	
		glm::mat4 model = Scion::Core::RSTModel( transform, boxCollider.width, boxCollider.height );

		auto color = Color{ 255, 0, 0, 135 };
		bool bUseIso{ false }; // We need another way to determine if we are using iso coords. The user might want to
							   // use their own physics
		if ( registry.GetRegistry().all_of<PhysicsComponent>( entity ) )
		{
			auto& physics = registry.GetRegistry().get<PhysicsComponent>( entity );
			const auto& attrs = physics.GetAttributes();
			if ( physics.IsSensor() )
			{
				color = Color{ 0, 255, 0, 135 };
			}
			// Tilemap Editor does not create the rigid body
			else if ( attrs.bIsSensor )
			{
				color = Color{ 0, 255, 0, 135 };
			}

			// If not a box or circle, consider this an iso shape
			if ( !attrs.bBoxShape && !attrs.bCircle )
			{
				bUseIso = true;
			}
		}

		const float width = static_cast<float>( boxCollider.width );
		const float height = static_cast<float>( boxCollider.height );
		const glm::vec2 position{ transform.position.x + boxCollider.offset.x,
								  transform.position.y + boxCollider.offset.y };

		if ( bUseIso )
		{
			// Diamond: top, left, bottom, right
			const float cx = position.x + width * 0.5f;
			const float cy = position.y + height * 0.5f;
			const float halfW = width * 0.5f;
			const float isoHeight = -height;

			m_pShapeRenderer->AddQuad( { cx, cy },
									   { cx - halfW, cy + isoHeight * 0.5f },
									   { cx, cy + isoHeight },
									   { cx + halfW, cy + isoHeight * 0.5f },
									   model,
									   color,
									   LAYER_BOX );
		}
		else
		{
			m_pShapeRenderer->AddRect( position, width, height, model, color, LAYER_BOX );
		}
	}

	const Color circleColor{ 0, 255, 0, 135 };
	auto circleView = registry.GetRegistry().view<TransformComponent, CircleColliderComponent>();
	for ( auto entity : circleView )
	{
		const auto& transform = circleView.get<TransformComponent>( entity );
		const auto& circleCollider = circleView.get<CircleColliderComponent>( entity );

		const float radiusX = circleCollider.radius * transform.scale.x;
		const float radiusY = circleCollider.radius * transform.scale.y;

		// The collider offset is the top left of the circle's bounding box
		const glm::vec2 center{ transform.position.x + circleCollider.offset.x + radiusX,
								transform.position.y + circleCollider.offset.y + radiusY };

		// Unit circle stretched to an ellipse so non uniform scale matches the old circle quad
		glm::mat4 model{ 1.f };
		model[ 0 ][ 0 ] = radiusX;
		model[ 1 ][ 1 ] = radiusY;
		model[ 3 ][ 0 ] = center.x;
		model[ 3 ][ 1 ] = center.y;

		m_pShapeRenderer->AddCircle(
			1.f, ShapeRenderer::CircleSegments( std::max( radiusX, radiusY ) ), model, circleColor, LAYER_CIRCLE );
	}

	// The shader must stay enabled until Render, End can flush full windows
	m_pShapeRenderer->End();
	m_pShapeRenderer->Render();
	colorShader->Disable();
}
} // namespace Scion::Core::Systems
