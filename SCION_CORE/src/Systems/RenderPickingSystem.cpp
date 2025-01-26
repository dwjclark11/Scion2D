#include "Core/Systems/RenderPickingSystem.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/Components/SpriteComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/PickingBatchRenderer.h"

#include "Logger/Logger.h"

using namespace SCION_RENDERING;
using namespace SCION_CORE::ECS;

namespace SCION_CORE::Systems
{
RenderPickingSystem::RenderPickingSystem()
	: m_pBatchRenderer{ std::make_unique<PickingBatchRenderer>() }
{
}

RenderPickingSystem::~RenderPickingSystem()
{
}

void RenderPickingSystem::Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera )
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	const auto pickingShader = assetManager.GetShader( "picking" );
	auto cam_mat = camera.GetCameraMatrix();

	if ( !pickingShader )
	{
		SCION_ERROR( "Sprite shader program has not been set correctly!" );
		return;
	}

	// enable the shader
	pickingShader->Enable();
	pickingShader->SetUniformMat4( "uProjection", cam_mat );

	m_pBatchRenderer->Begin();
	auto spriteView = registry.GetRegistry().view<SpriteComponent, TransformComponent>();
	for ( auto entity : spriteView )
	{
		const auto& transform = spriteView.get<TransformComponent>( entity );
		const auto& sprite = spriteView.get<SpriteComponent>( entity );

		if ( !SCION_CORE::EntityInView( transform, sprite.width, sprite.height, camera ) )
			continue;

		if ( sprite.texture_name.empty() || sprite.bHidden )
			continue;

		auto pTexture = assetManager.GetTexture( sprite.texture_name );
		if ( !pTexture )
		{
			SCION_ERROR( "Texture [{0}] was not created correctly!", sprite.texture_name );
			return;
		}

		glm::vec4 spriteRect{ transform.position.x,
							  transform.position.y,
							  sprite.width,
							  sprite.height };

		glm::vec4 uvRect{ sprite.uvs.u, sprite.uvs.v, sprite.uvs.uv_width, sprite.uvs.uv_height };
		glm::mat4 model = SCION_CORE::RSTModel( transform, sprite.width, sprite.height );

		m_pBatchRenderer->AddSprite(
			spriteRect, uvRect, pTexture->GetID(), sprite.layer, static_cast<uint32_t>( entity ), sprite.color, model );
	}

	m_pBatchRenderer->End();
	m_pBatchRenderer->Render();

	pickingShader->Disable();
}
} // namespace SCION_CORE::Systems
