#include "Core/Systems/RenderUISystem.h"
#include "Core/ECS/Components/TextComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/CoreUtilities.h"

#include <Rendering/Essentials/Font.h>
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>

#include <Rendering/Core/BatchRenderer.h>
#include <Rendering/Core/TextBatchRenderer.h>
#include <Rendering/Core/Camera2D.h>

#include <Logger/Logger.h>

using namespace SCION_CORE::ECS;
using namespace SCION_RESOURCES;

namespace SCION_CORE::Systems
{

RenderUISystem::RenderUISystem()
	: m_pSpriteRenderer{ std::make_unique<SCION_RENDERING::SpriteBatchRenderer>() }
	, m_pTextRenderer{ std::make_unique<SCION_RENDERING::TextBatchRenderer>() }
	, m_pCamera2D{ nullptr }
{
	auto& coreEngine = CoreEngineData::GetInstance();

	m_pCamera2D = std::make_unique<SCION_RENDERING::Camera2D>( coreEngine.WindowWidth(), coreEngine.WindowHeight() );

	m_pCamera2D->Update();
}

RenderUISystem::~RenderUISystem()
{
}

void RenderUISystem::Update( SCION_CORE::ECS::Registry& registry )
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	auto pSpriteShader = assetManager.GetShader( "basic" );
	if ( !pSpriteShader )
	{
		SCION_ERROR( "Failed to Render UI, basic shader is invalid" );
		return;
	}

	auto& reg = registry.GetRegistry();
	auto spriteView = reg.view<UIComponent, SpriteComponent, TransformComponent>();

	auto cam_mat = m_pCamera2D->GetCameraMatrix();
	pSpriteShader->Enable();
	pSpriteShader->SetUniformMat4( "uProjection", cam_mat );

	m_pSpriteRenderer->Begin();

	for ( auto entity : spriteView )
	{
		const auto& transform = spriteView.get<TransformComponent>( entity );
		const auto& sprite = spriteView.get<SpriteComponent>( entity );

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

		m_pSpriteRenderer->AddSprite( spriteRect, uvRect, pTexture->GetID(), sprite.layer, model, sprite.color );
	}

	m_pSpriteRenderer->End();
	m_pSpriteRenderer->Render();

	pSpriteShader->Disable();

	// If there are no entities in the view, leave
	auto textView = reg.view<TextComponent, TransformComponent>();
	if ( textView.size_hint() < 1 )
		return;

	auto pFontShader = assetManager.GetShader( "font" );

	if ( !pFontShader )
	{
		SCION_ERROR( "Failed to get the font shader from the asset manager!" );
		return;
	}

	pFontShader->Enable();
	pFontShader->SetUniformMat4( "uProjection", cam_mat );

	m_pTextRenderer->Begin();

	for ( auto entity : textView )
	{
		auto& text = textView.get<TextComponent>( entity );
		if ( text.sFontName.empty() || text.bHidden )
			continue;

		const auto& pFont = assetManager.GetFont( text.sFontName );
		if ( !pFont )
		{
			SCION_ERROR( "Font [{}] does not exist in the asset manager!", text.sFontName );
			continue;
		}

		const auto& transform = textView.get<TransformComponent>( entity );

		if ( transform.bDirty || text.bDirty )
		{
			const auto [ textWidth, textHeight ] = SCION_CORE::GetTextBlockSize( text, transform, assetManager );
			text.textBoxWidth = textWidth;
			text.textBoxHeight = textHeight;
		}

		glm::mat4 model = SCION_CORE::RSTModel( transform, text.textBoxWidth, text.textBoxHeight );

		m_pTextRenderer->AddText(
			text.sTextStr, pFont, transform.position, text.padding, text.wrap, text.color, model );
	}

	m_pTextRenderer->End();
	m_pTextRenderer->Render();

	pFontShader->Disable();
}

void RenderUISystem::CreateRenderUISystemLuaBind( sol::state& lua )
{
	lua.new_usertype<RenderUISystem>( "RenderUISystem",
									  sol::call_constructor,
									  sol::constructors<RenderUISystem()>(),
									  "update",
									  [ & ]( RenderUISystem& system, Registry& reg ) { system.Update( reg ); } );
}

} // namespace SCION_CORE::Systems
