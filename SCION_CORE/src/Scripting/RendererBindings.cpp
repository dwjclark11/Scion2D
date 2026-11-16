#include "Core/Scripting/RendererBindings.h"
#include "Core/Resources/AssetManager.h"
#include "Core/ECS/MainRegistry.h"
#include <Rendering/Essentials/Primitives.h>
#include <Rendering/Essentials/Shader.h>
#include <Rendering/Essentials/Texture.h>
#include <Rendering/Buffers/Framebuffer.h>
#include <Rendering/Core/Camera2D.h>
#include <Rendering/Core/Renderer.h>
#include "Core/ECS/Registry.h"
#include <Logger/Logger.h>

using namespace Scion::Rendering;
using namespace Scion::Resources;

namespace Scion::Core::Scripting
{

void RendererBinder::CreateRenderingBind( sol::state& lua, Scion::Core::ECS::Registry& registry )
{
	// Get the Asset Manager
	auto& mainRegistry = MAIN_REGISTRY();
	auto& assetManager = mainRegistry.GetAssetManager();

	lua.new_usertype<Text>(
		"Text",
		sol::call_constructor,
		sol::factories( [ & ]( const glm::vec2& position,
							   const std::string& textStr,
							   const std::string& fontName,
							   float wrap,
							   const Color& color ) {
			auto pFont = assetManager.GetFont( fontName );
			if ( !pFont )
			{
				SCION_ERROR( "Failed to get font [{}] -- Does not exist in asset manager!", fontName );
				return Text{};
			}

			return Text{ .position = position, .textStr = textStr, .wrap = wrap, .pFont = pFont, .color = color };
		} ),
		"position",
		&Text::position,
		"textStr",
		&Text::textStr,
		"wrap",
		&Text::wrap,
		"color",
		&Text::color );

	auto& renderer = mainRegistry.GetContext<std::unique_ptr<Renderer>>();
	if ( !renderer )
	{
		SCION_ERROR( "Failed to bind the Renderer to Lua! Not in the registry context!" );
		return;
	}

	auto& camera = registry.GetContext<std::unique_ptr<Camera2D>>();
	if ( !camera )
	{
		SCION_ERROR( "Failed to bind the camera to lua - Failed to get the camera from the registry context!" );
		return;
	}

	Renderer::CreateLuaBind( lua, *renderer );
	Camera2D::CreateLuaBind( lua, *camera );
	Shader::CreateLuaBind( lua );
	Texture::CreateLuaBind( lua );
	Framebuffer::CreateLuaBind( lua );
}

} // namespace Scion::Core::Scripting
