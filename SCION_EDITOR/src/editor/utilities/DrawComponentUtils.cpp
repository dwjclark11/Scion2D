#include "DrawComponentUtils.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "ScionUtilities/ScionUtilities.h"
#include "Logger/Logger.h"

#include "editor/utilities/ImGuiUtils.h"

using namespace SCION_CORE::ECS;

namespace SCION_EDITOR
{
void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::TransformComponent& transform )
{
	ImGui::SeparatorText( "Transform" );
	ImGui::PushID( entt::type_hash<TransformComponent>::value() );
	if ( ImGui::TreeNodeEx( "##TransformTree", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "position" );
		ImGui::ColoredLabel( "x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		ImGui::InputFloat( "##position_x", &transform.position.x, 1.f, 10.f, "%.1f" );
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		ImGui::InputFloat( "##position_y", &transform.position.y, 1.f, 10.f, "%.1f" );

		ImGui::InlineLabel( "scale" );
		ImGui::ColoredLabel( "x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##scale_x", &transform.scale.x, 1.f, 1.f, "%.1f" ) )
		{
			transform.scale.x = std::clamp( transform.scale.x, 0.1f, 150.f );
		}
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##scale_y", &transform.scale.y, 1.f, 1.f, "%.1f" ) )
		{
			transform.scale.y = std::clamp( transform.scale.y, 0.1f, 150.f );
		}

		ImGui::InlineLabel( "rotation" );
		ImGui::InputFloat( "##rotation", &transform.rotation, 1.f, 1.f, "%.1f" );
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::SpriteComponent& sprite )
{
	bool bChanged{ false };

	ImGui::SeparatorText( "Sprite" );
	ImGui::PushID( entt::type_hash<SpriteComponent>::value() );
	if ( ImGui::TreeNodeEx( "##SpriteTree", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::AddSpaces( 2 );
		if ( ImGui::BeginDragDropTarget() )
		{
			const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload( "AssetDisplayTexture" );

			if ( pPayload )
			{
				const char* texture = (const char*)pPayload->Data;
				std::string textureStr{ texture };
				SCION_ASSERT( !textureStr.empty() && "Texture Name is Empty!" );
				if ( !textureStr.empty() )
				{
					sprite.texture_name = textureStr;
				}
			}

			ImGui::EndDragDropTarget();
		}

		// Color picker
		ImVec4 col = { sprite.color.r / 255.f, sprite.color.g / 255.f, sprite.color.b / 255.f, sprite.color.a / 255.f };
		ImGui::InlineLabel( "color" );
		ImGui::ItemToolTip( "Sprite color override." );
		if ( ImGui::ColorEdit4( "##color", &col.x, IMGUI_COLOR_PICKER_FLAGS ) )
		{
			sprite.color.r = static_cast<GLubyte>( col.x * 255.f );
			sprite.color.g = static_cast<GLubyte>( col.y * 255.f );
			sprite.color.b = static_cast<GLubyte>( col.z * 255.f );
			sprite.color.a = static_cast<GLubyte>( col.w * 255.f );
		}

		auto& assetManager = MAIN_REGISTRY().GetAssetManager();

		std::string sSelectedTexture{ sprite.texture_name };
		ImGui::InlineLabel( "texture" );
		ImGui::ItemToolTip( "The current active texture of the sprite to be drawn." );
		if ( ImGui::BeginCombo( "##texture", sSelectedTexture.c_str() ) )
		{
			for ( const auto& sTextureName : assetManager.GetAssetKeyNames( SCION_UTIL::AssetType::TEXTURE ) )
			{
				if ( ImGui::Selectable( sTextureName.c_str(), sTextureName == sSelectedTexture ) )
				{
					sSelectedTexture = sTextureName;
					sprite.texture_name = sSelectedTexture;
					bChanged = true;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::AddSpaces( 2 );

		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "width" );
		ImGui::ItemToolTip( "The width of the sprite. This is used in UV calculations." );
		if ( ImGui::InputFloat( "##width", &sprite.width, 8.f, 8.f ) )
		{
			sprite.width = std::clamp( sprite.width, 8.f, 1366.f );
			bChanged = true;
		}

		ImGui::InlineLabel( "height" );
		ImGui::ItemToolTip( "The height of the sprite. This is used in UV calculations." );
		if ( ImGui::InputFloat( "##height", &sprite.height, 8.f, 8.f ) )
		{
			sprite.height = std::clamp( sprite.height, 8.f, 768.f );
			bChanged = true;
		}

		ImGui::InlineLabel( "layer" );
		ImGui::ItemToolTip( "Z-Index in which to draw the sprite." );
		if ( ImGui::InputInt( "##layer", &sprite.layer, 1, 1 ) )
		{
			sprite.layer = std::clamp( sprite.layer, 0, 10 );
		}

		ImGui::InlineLabel( "start pos" );
		ImGui::ItemToolTip( "The index positions where we want to start our UV calculations." );
		ImGui::ColoredLabel( "x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		if ( ImGui::InputInt( "##start_x", &sprite.start_x, 1, 1 ) )
		{
			sprite.start_x = std::clamp( sprite.start_x, 0, 32 );
			bChanged = true;
		}
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if ( ImGui::InputInt( "##start_y", &sprite.start_y, 1, 1 ) )
		{
			sprite.start_y = std::clamp( sprite.start_y, 0, 32 );
			bChanged = true;
		}

		ImGui::TreePop();
		ImGui::PopItemWidth();
	}

	ImGui::PopID();

	if ( bChanged )
	{
		auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sprite.texture_name );
		if ( !pTexture )
			return;

		SCION_CORE::GenerateUVs( sprite, pTexture->GetWidth(), pTexture->GetHeight() );
	}
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::AnimationComponent& animation )
{
	ImGui::SeparatorText( "Animation Component" );
	ImGui::PushID( entt::type_hash<AnimationComponent>::value() );
	if ( ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "num frames" );
		if ( ImGui::InputInt( "##1", &animation.numFrames, 1, 1 ) )
			animation.numFrames = std::clamp( animation.numFrames, 1, 15 );

		ImGui::InlineLabel( "frame rate" );
		if ( ImGui::InputInt( "##frame_rate", &animation.frameRate, 1, 1 ) )
			animation.frameRate = std::clamp( animation.frameRate, 1, 25 );

		ImGui::InlineLabel( "frame offset" );
		if ( ImGui::InputInt( "##frame_offset", &animation.frameOffset, 1, 1 ) )
			animation.frameOffset = std::clamp( animation.frameOffset, 0, 15 );

		ImGui::InlineLabel( "vertical" );
		ImGui::ItemToolTip( "Does the sprite animations scroll vertically?" );
		ImGui::Checkbox( "##vertical", &animation.bVertical );
		ImGui::InlineLabel( "looped" );
		ImGui::ItemToolTip( "Are the sprite animatons to be looped?" );
		ImGui::Checkbox( "##looped", &animation.bLooped );
		ImGui::TreePop();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::BoxColliderComponent& boxCollider )
{
	ImGui::SeparatorText( "Box Collider Component" );
	ImGui::PushID( entt::type_hash<BoxColliderComponent>::value() );
	if ( ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "width" );
		if ( ImGui::InputInt( "##width", &boxCollider.width, 4, 4 ) )
			boxCollider.width = std::clamp( boxCollider.width, 4, 1366 );

		ImGui::InlineLabel( "height" );
		if ( ImGui::InputInt( "##height", &boxCollider.height, 4, 4 ) )
			boxCollider.height = std::clamp( boxCollider.height, 4, 768 );

		ImGui::InlineLabel( "offset" );
		ImGui::ItemToolTip( "The offset of the box collider from the origin. Origin is the TL corner." );
		ImGui::ColoredLabel( "x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##offset_x", &boxCollider.offset.x, 4.f, 4.f ) )
			boxCollider.offset.x = std::clamp( boxCollider.offset.x, 0.f, 128.f );
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##offset_y", &boxCollider.offset.y, 4.f, 4.f ) )
			boxCollider.offset.y = std::clamp( boxCollider.offset.y, 0.f, 128.f );
		ImGui::TreePop();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::CircleColliderComponent& circleCollider )
{
	ImGui::SeparatorText( "Circle Collider Component" );
	ImGui::PushID( entt::type_hash<CircleColliderComponent>::value() );
	if ( ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "radius" );
		if ( ImGui::InputFloat( "##radius", &circleCollider.radius, 4, 4 ) )
			circleCollider.radius = std::clamp( circleCollider.radius, 4.f, 1366.f );

		ImGui::InlineLabel( "offset" );
		ImGui::ItemToolTip( "The offset of the circle collider from the origin. Origin is the TL corner." );
		ImGui::ColoredLabel( "x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##offset_x", &circleCollider.offset.x, 4.f, 4.f ) )
			circleCollider.offset.x = std::clamp( circleCollider.offset.x, 0.f, 128.f );
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##offset_y", &circleCollider.offset.y, 4.f, 4.f ) )
			circleCollider.offset.y = std::clamp( circleCollider.offset.y, 0.f, 128.f );
		ImGui::TreePop();
		ImGui::PopItemWidth();
	}
	ImGui::PopID();
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::PhysicsComponent& physics )
{
	ImGui::SeparatorText( "Physics Component" );
	ImGui::PushID( entt::type_hash<PhysicsComponent>::value() );
	if ( ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		// TODO:
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::RigidBodyComponent& rigidbody )
{
	ImGui::SeparatorText( "Rigidbody Component" );
	ImGui::PushID( entt::type_hash<RigidBodyComponent>::value() );
	if ( ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "max velocity" );
		ImGui::ColoredLabel( "x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		ImGui::InputFloat( "##maxVelocity_x", &rigidbody.maxVelocity.x, 1.f, 10.f, "%.1f" );
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		ImGui::InputFloat( "##maxVelocity_y", &rigidbody.maxVelocity.y, 1.f, 10.f, "%.1f" );

		ImGui::TreePop();
	}
	ImGui::PopID();
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::TextComponent& textComponent )
{
	ImGui::SeparatorText( "Text Component" );
	ImGui::PushID( entt::type_hash<TextComponent>::value() );
	if ( ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		std::string sTextBuffer{ textComponent.sTextStr };
		ImGui::InlineLabel( "text" );
		if ( ImGui::InputText(
				 "##_textStr", sTextBuffer.data(), sizeof( char ) * 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			textComponent.sTextStr = std::string{ sTextBuffer.data() };
		}

		std::string sFontName{ textComponent.sFontName };
		ImGui::PushItemWidth( 164.f );
		ImGui::InlineLabel( "font" );
		if ( ImGui::BeginCombo( "##fontName", sFontName.c_str() ) )
		{
			auto& assetManager = MAIN_REGISTRY().GetAssetManager();
			for ( const auto& sFont : assetManager.GetAssetKeyNames( SCION_UTIL::AssetType::FONT ) )
			{
				if ( ImGui::Selectable( sFont.c_str(), sFont == sFontName ) )
				{
					sFontName = sFont;
					textComponent.sFontName = sFontName;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "padding" );
		if ( ImGui::InputInt( "##padding", &textComponent.padding, 0, 0 ) )
		{
			// TODO
		}

		ImGui::InlineLabel( "wrap" );
		if ( ImGui::InputFloat( "##textWrap", &textComponent.wrap, 0.f, 0.f ) )
		{
			// TODO
		}

		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Identification& identification )
{
	ImGui::SeparatorText( "Identificaton" );
	ImGui::PushID( entt::type_hash<Identification>::value() );
	if ( ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		std::string sNameBuffer{ identification.name };
		ImGui::InlineLabel( "name" );
		if ( ImGui::InputText(
				 "##_name", sNameBuffer.data(), sizeof( char ) * 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			identification.name = std::string{ sNameBuffer.data() };
		}

		std::string sGroupBuffer{ identification.group };
		ImGui::InlineLabel( "group" );
		if ( ImGui::InputText(
				 "##_group", sGroupBuffer.data(), sizeof( char ) * 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			identification.group = std::string{ sGroupBuffer.data() };
		}

		ImGui::TreePop();
	}
	ImGui::PopID();
}

} // namespace SCION_EDITOR
