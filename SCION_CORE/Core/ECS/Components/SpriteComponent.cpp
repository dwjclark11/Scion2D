#include "SpriteComponent.h"
#include "../../Resources/AssetManager.h"
#include <Logger/Logger.h>

using namespace SCION_RESOURCES;

void SCION_CORE::ECS::SpriteComponent::CreateSpriteLuaBind(sol::state& lua, SCION_CORE::ECS::Registry& registry)
{
	lua.new_usertype<SpriteComponent>(
		"Sprite",
		"type_id", &entt::type_hash<SpriteComponent>::value,
		sol::call_constructor,
		sol::factories(
			[](const std::string& textureName, float width, float height, int start_x, int start_y, int layer) {
				return SpriteComponent{
					.width = width,
					.height = height,
					.uvs = UVs{},
					.color = SCION_RENDERING::Color{255, 255, 255, 255},
					.start_x = start_x,
					.start_y = start_y,
					.layer = layer,
					.texture_name = textureName
				};
			}
		),
		"texture_name", &SpriteComponent::texture_name,
		"width", &SpriteComponent::width,
		"height", &SpriteComponent::height,
		"start_x", &SpriteComponent::start_x,
		"start_y", &SpriteComponent::start_y,
		"layer", &SpriteComponent::layer,
		"generate_uvs", [&](SpriteComponent& sprite) {
			auto& assetManager = registry.GetContext<std::shared_ptr<AssetManager>>();
			auto& texture = assetManager->GetTexture(sprite.texture_name);

			if (texture.GetID() == 0)
			{
				SCION_ERROR("Failed to generate uvs -- Texture [{}] -- Does not exists or invalid", sprite.texture_name);
				return;
			}

			sprite.generate_uvs(texture.GetWidth(), texture.GetHeight());
		}
	);
}
