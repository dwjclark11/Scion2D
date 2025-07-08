#include "Core/ECS/Components/SpriteComponent.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include <Rendering/Essentials/Texture.h>
#include <Logger/Logger.h>

using namespace SCION_RESOURCES;

std::string SCION_CORE::ECS::SpriteComponent::to_string() const
{
	std::stringstream ss;
	ss << "==== Sprite Component ==== \n"
	   << std::boolalpha << "Texture Name: " << sTextureName << "\n"
	   << "Width: " << width << "\n"
	   << "Height: " << height << "\n"
	   << "StartX: " << start_x << "\n"
	   << "StartY: " << start_y << "\n"
	   << "Layer: " << layer << "\n"
	   << "UVs: \n\t"
	   << "U: " << uvs.u << "\n\t"
	   << "V: " << uvs.v << "\n\t"
	   << "UvWidth: " << uvs.uv_width << "\n\t"
	   << "UvHeight: " << uvs.uv_width << "\n"
	   << "Color: \n\t"
	   << "Red: " << color.r << "\n\t"
	   << "Green: " << color.g << "\n\t"
	   << "Blue: " << color.b << "\n\t"
	   << "Alpha: " << color.a << "\n"
	   << "bHidden: " << bHidden << "\n";

	return ss.str();
}

void SCION_CORE::ECS::SpriteComponent::CreateSpriteLuaBind( sol::state& lua )
{
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	lua.new_usertype<SCION_RENDERING::Color>( "Color",
											  sol::call_constructor,
											  sol::factories( []( GLubyte r, GLubyte g, GLubyte b, GLubyte a ) {
												  return SCION_RENDERING::Color{ .r = r, .g = g, .b = b, .a = a };
											  } ),
											  "r",
											  &SCION_RENDERING::Color::r,
											  "g",
											  &SCION_RENDERING::Color::g,
											  "b",
											  &SCION_RENDERING::Color::b,
											  "a",
											  &SCION_RENDERING::Color::a );

	lua.new_usertype<UVs>( "UVs",
						   sol::call_constructor,
						   sol::factories( []( float u, float v ) { return UVs{ .u = u, .v = v }; } ),
						   "u",
						   &UVs::u,
						   "v",
						   &UVs::v,
						   "uvWidth",
						   &UVs::uv_width,
						   "uvHeight",
						   &UVs::uv_height );

	lua.new_usertype<SpriteComponent>(
		"Sprite",
		"type_id",
		&entt::type_hash<SpriteComponent>::value,
		sol::call_constructor,
		sol::factories(
			[]( const std::string& textureName, float width, float height, int start_x, int start_y, int layer ) {
				return SpriteComponent{ .sTextureName = textureName,
										.width = width,
										.height = height,
										.uvs = UVs{},
										.color = SCION_RENDERING::Color{ 255, 255, 255, 255 },
										.start_x = start_x,
										.start_y = start_y,
										.layer = layer };
			} ),
		"sTextureName",
		&SpriteComponent::sTextureName,
		"width",
		&SpriteComponent::width,
		"height",
		&SpriteComponent::height,
		"startX",
		&SpriteComponent::start_x,
		"startY",
		&SpriteComponent::start_y,
		"layer",
		&SpriteComponent::layer,
		"bHidden",
		&SpriteComponent::bHidden,
		"uvs",
		&SpriteComponent::uvs,
		"color",
		&SpriteComponent::color,
		"generateUVs",
		[ & ]( SpriteComponent& sprite ) {
			auto pTexture = assetManager.GetTexture( sprite.sTextureName );

			if ( !pTexture )
			{
				SCION_ERROR( "Failed to generate uvs -- Texture [{}] -- Does not exists or invalid",
							 sprite.sTextureName );
				return;
			}

			SCION_CORE::GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );
		},
		"inspectUVs",
		[]( SpriteComponent& sprite ) {
			sprite.uvs.u = sprite.start_x * sprite.uvs.uv_width;
			sprite.uvs.v = sprite.start_y * sprite.uvs.uv_height;
		},
		"inspectX",
		[]( SpriteComponent& sprite ) { sprite.uvs.u = sprite.start_x * sprite.uvs.uv_width; },
		"inspectY",
		[]( SpriteComponent& sprite ) { sprite.uvs.v = sprite.start_y * sprite.uvs.uv_height; } );
}
