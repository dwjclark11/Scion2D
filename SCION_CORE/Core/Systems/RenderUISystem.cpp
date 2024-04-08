#include "RenderUISystem.h"
#include "../ECS/Components/TextComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include <Rendering/Essentials/Font.h>
#include "../Resources/AssetManager.h"
#include <Logger/Logger.h>
#include "../CoreUtilities/CoreEngineData.h"
#include "../CoreUtilities/CoreUtilities.h"

using namespace SCION_CORE::ECS;
using namespace SCION_RESOURCES;

namespace SCION_CORE::Systems {

	RenderUISystem::RenderUISystem(SCION_CORE::ECS::Registry& registry)
		: m_Registry{registry}
		, m_pSpriteRenderer{ std::make_unique<SCION_RENDERING::SpriteBatchRenderer>()}
		, m_pTextRenderer{ std::make_unique<SCION_RENDERING::TextBatchRenderer>()}
		, m_pCamera2D{ nullptr }
	{
		auto& coreEngine = CoreEngineData::GetInstance();

		m_pCamera2D = std::make_unique<SCION_RENDERING::Camera2D>(
			coreEngine.WindowWidth(), 
			coreEngine.WindowHeight()
		);

		m_pCamera2D->Update();
	}

	void RenderUISystem::Update(entt::registry& registry)
	{
		// If there are no entities in the view, leave
		auto textView = registry.view<TextComponent, TransformComponent>();
		if (textView.size_hint() < 1)
			return;

		auto& assetManager = m_Registry.GetContext<std::shared_ptr<AssetManager>>();
		auto pFontShader = assetManager->GetShader("font");

		if (!pFontShader)
		{
			SCION_ERROR("Failed to get the font shader from the asset manager!");
			return;
		}

		auto cam_mat = m_pCamera2D->GetCameraMatrix();
		

		pFontShader->Enable();
		pFontShader->SetUniformMat4("uProjection", cam_mat);

		m_pTextRenderer->Begin();

		for (auto entity : textView)
		{
			const auto& text = textView.get<TextComponent>(entity);

			if (text.sFontName.empty() || text.bHidden)
				continue;

			const auto& pFont = assetManager->GetFont(text.sFontName);
			if (!pFont)
			{
				SCION_ERROR("Font [{}] does not exist in the asset manager!", text.sFontName);
				continue;
			}

			const auto& transform = textView.get<TransformComponent>(entity);
			const auto fontSize = pFont->GetFontSize();

			glm::mat4 model = SCION_CORE::RSTModel(transform, fontSize, fontSize);
			m_pTextRenderer->AddText(text.sTextStr, pFont, transform.position, text.padding, text.wrap, text.color, model);
		}

		m_pTextRenderer->End();
		m_pTextRenderer->Render();

		pFontShader->Disable();
	}
}
