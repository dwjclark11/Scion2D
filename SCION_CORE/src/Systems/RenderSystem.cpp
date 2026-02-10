#include "Core/Systems/RenderSystem.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include <Rendering/Core/Camera2D.h>
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>
#include <Rendering/Core/BatchRenderer.h>

#include "ScionUtilities/HelperUtilities.h"

#include <Logger/Logger.h>

#include <ranges>

using namespace Scion::Core::ECS;
using namespace Scion::Rendering;
using namespace SCION_RESOURCES;

namespace Scion::Core::Systems
{
RenderSystem::RenderSystem()
	: m_pBatchRenderer{ std::make_unique<SpriteBatchRenderer>() }
{
}

RenderSystem::~RenderSystem() = default;

void RenderSystem::Update( Scion::Core::ECS::Registry& registry, Scion::Rendering::Camera2D& camera )
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	auto spriteShader = assetManager.GetShader( "basic" );
	auto cam_mat = camera.GetCameraMatrix();

	if ( spriteShader->ShaderProgramID() == 0 )
	{
		SCION_ERROR( "Sprite shader program has not been set correctly!" );
		return;
	}

	// enable the shader
	spriteShader->Enable();
	spriteShader->SetUniformMat4( "uProjection", cam_mat );

	m_pBatchRenderer->Begin();

	auto spriteView = registry.GetRegistry().view<SpriteComponent, TransformComponent>( entt::exclude<UIComponent> );

	for ( const auto entity : spriteView )
	{
		const auto& transform = spriteView.get<TransformComponent>( entity );
		const auto& sprite = spriteView.get<SpriteComponent>( entity );

		if ( !Scion::Core::EntityInView( transform, sprite.width, sprite.height, camera ) )
			continue;

		if ( sprite.sTextureName.empty() || sprite.bHidden )
			continue;

		const auto& pTexture = assetManager.GetTexture( sprite.sTextureName );
		if ( !pTexture )
		{
			SCION_ERROR( "Texture [{0}] was not created correctly!", sprite.sTextureName );
			return;
		}

		glm::vec4 spriteRect{ transform.position.x, transform.position.y, sprite.width, sprite.height };
		glm::vec4 uvRect{ sprite.uvs.u, sprite.uvs.v, sprite.uvs.uv_width, sprite.uvs.uv_height };

		glm::mat4 model = Scion::Core::RSTModel( transform, sprite.width, sprite.height );

		if ( sprite.bIsoMetric )
		{
			m_pBatchRenderer->AddSpriteIso( spriteRect,
											uvRect,
											pTexture->GetID(),
											sprite.isoCellX,
											sprite.isoCellY,
											sprite.layer,
											model,
											sprite.color );
		}
		else
		{
			m_pBatchRenderer->AddSprite( spriteRect, uvRect, pTexture->GetID(), sprite.layer, model, sprite.color );
		}
	}

	m_pBatchRenderer->End();
	m_pBatchRenderer->Render();

	spriteShader->Disable();
}

void RenderSystem::CreateRenderSystemLuaBind( sol::state& lua, Scion::Core::ECS::Registry& registry )
{
	auto& pCamera = registry.GetContext<std::shared_ptr<Camera2D>>();

	SCION_ASSERT( pCamera && "A camera must exist in the current scene!" );

	lua.new_usertype<RenderSystem>( "RenderSystem",
									sol::call_constructor,
									sol::constructors<RenderSystem()>(),
									"update",
									[ & ]( RenderSystem& system, Registry& reg ) { system.Update( reg, *pCamera ); } );
}

} // namespace Scion::Core::Systems
