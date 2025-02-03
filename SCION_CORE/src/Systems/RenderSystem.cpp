#include "Core/Systems/RenderSystem.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include <Rendering/Core/Camera2D.h>
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Core/BatchRenderer.h>

#include "ScionUtilities/HelperUtilities.h"

#include <Logger/Logger.h>

#include <ranges>

using namespace SCION_CORE::ECS;
using namespace SCION_RENDERING;
using namespace SCION_RESOURCES;

namespace SCION_CORE::Systems
{
RenderSystem::RenderSystem()
	: m_pBatchRenderer{ std::make_unique<SpriteBatchRenderer>() }
{
}

void RenderSystem::Update( SCION_CORE::ECS::Registry& registry, SCION_RENDERING::Camera2D& camera,
						   const std::vector<SCION_UTIL::SpriteLayerParams>& layerFilters )
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
	std::function<bool( entt::entity )> filterFunc;

	// Check to see if the layers are visible, if not, filter them out.
	if ( layerFilters.empty() )
	{
		filterFunc = []( entt::entity ) { return true; };
	}
	else
	{
		filterFunc = [ & ]( entt::entity entity ) {
			// We only want to filter tiles
			if ( !registry.GetRegistry().all_of<TileComponent>( entity ) )
				return true;

			const auto& sprite = spriteView.get<SpriteComponent>( entity );
			if ( sprite.layer >= 0 && sprite.layer < layerFilters.size() )
			{
				return layerFilters[ sprite.layer ].bVisible;
			}

			return false;
		};
	}

	for ( const auto& entity : std::views::filter( spriteView, filterFunc ) )
	{
		const auto& transform = spriteView.get<TransformComponent>( entity );
		const auto& sprite = spriteView.get<SpriteComponent>( entity );

		if ( !SCION_CORE::EntityInView( transform, sprite.width, sprite.height, camera ) )
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

		glm::mat4 model = SCION_CORE::RSTModel( transform, sprite.width, sprite.height );

		m_pBatchRenderer->AddSprite( spriteRect, uvRect, pTexture->GetID(), sprite.layer, model, sprite.color );
	}

	m_pBatchRenderer->End();
	m_pBatchRenderer->Render();

	spriteShader->Disable();
}

void RenderSystem::CreateRenderSystemLuaBind( sol::state& lua, SCION_CORE::ECS::Registry& registry )
{
	auto& pCamera = registry.GetContext<std::shared_ptr<Camera2D>>();

	SCION_ASSERT( pCamera && "A camera must exist in the current scene!" );

	lua.new_usertype<RenderSystem>( "RenderSystem",
									sol::call_constructor,
									sol::constructors<RenderSystem()>(),
									"update",
									[ & ]( RenderSystem& system, Registry& reg ) { system.Update( reg, *pCamera ); } );
}

} // namespace SCION_CORE::Systems
