#include "Core/Systems/RenderSystem.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/Components/SpriteComponent.h"
#include "Core/ECS/Components/TransformComponent.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include <Rendering/Core/Camera2D.h>
#include <Rendering/Essentials/Shader.h>
#include <Logger/Logger.h>

using namespace SCION_CORE::ECS;
using namespace SCION_RENDERING;
using namespace SCION_RESOURCES;

namespace SCION_CORE::Systems
{
RenderSystem::RenderSystem(SCION_CORE::ECS::Registry& registry)
	: m_Registry(registry)
	, m_pBatchRenderer{nullptr}
{
	m_pBatchRenderer = std::make_unique<SpriteBatchRenderer>();
}

void RenderSystem::Update()
{
	auto view = m_Registry.GetRegistry().view<SpriteComponent, TransformComponent>();
	if (view.size_hint() < 1)
		return;

	auto& camera = m_Registry.GetContext<std::shared_ptr<Camera2D>>();
	auto& assetManager = m_Registry.GetContext<std::shared_ptr<AssetManager>>();

	const auto& spriteShader = assetManager->GetShader("basic");
	auto cam_mat = camera->GetCameraMatrix();

	if (spriteShader->ShaderProgramID() == 0)
	{
		SCION_ERROR("Sprite shader program has not been set correctly!");
		return;
	}

	// enable the shader
	spriteShader->Enable();
	spriteShader->SetUniformMat4("uProjection", cam_mat);

	m_pBatchRenderer->Begin();

	for (const auto& entity : view)
	{
		const auto& transform = view.get<TransformComponent>(entity);
		const auto& sprite = view.get<SpriteComponent>(entity);

		if (!SCION_CORE::EntityInView(transform, sprite.width, sprite.height, *camera))
			continue;

		if (sprite.texture_name.empty() || sprite.bHidden)
			continue;

		const auto& texture = assetManager->GetTexture(sprite.texture_name);
		if (!texture)
		{
			SCION_ERROR("Texture [{0}] was not created correctly!", sprite.texture_name);
			return;
		}

		glm::vec4 spriteRect{transform.position.x, transform.position.y, sprite.width, sprite.height};
		glm::vec4 uvRect{sprite.uvs.u, sprite.uvs.v, sprite.uvs.uv_width, sprite.uvs.uv_height};

		glm::mat4 model = SCION_CORE::RSTModel(transform, sprite.width, sprite.height);

		m_pBatchRenderer->AddSprite(spriteRect, uvRect, texture->GetID(), sprite.layer, model, sprite.color);
	}

	m_pBatchRenderer->End();
	m_pBatchRenderer->Render();

	spriteShader->Disable();
}
} // namespace SCION_CORE::Systems