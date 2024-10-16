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
	ImGui::PushID(entt::type_hash<TransformComponent>::value());
	if (ImGui::TreeNodeEx("##TranformTree", ImGuiTreeNodeFlags_DefaultOpen))
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
		if (ImGui::InputFloat("##scale_x", &transform.scale.x, 1.f, 1.f, "%.1f"))
		{
			transform.scale.x = std::clamp( transform.scale.x, 0.1f, 150.f );
		}
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if (ImGui::InputFloat("##scale_y", &transform.scale.y, 1.f, 1.f, "%.1f"))
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
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pPayload = ImGui::AcceptDragDropPayload( "AssetDisplayTexture" );

			if (pPayload)
			{
				const char* texture = (const char*)pPayload->Data;
				std::string textureStr{ texture };
				SCION_ASSERT( !textureStr.empty() && "Texture Name is Empty!" );
				if (!textureStr.empty())
				{
					sprite.texture_name = textureStr;
				}
			}

			ImGui::EndDragDropTarget();
		}

		// Color picker
		ImVec4 col = { sprite.color.r / 255.f, sprite.color.g / 255.f, sprite.color.b / 255.f, sprite.color.a / 255.f };
		ImGui::InlineLabel( "color" );
		if (ImGui::ColorEdit4("##color", &col.x, IMGUI_COLOR_PICKER_FLAGS))
		{
			sprite.color.r = static_cast<GLubyte>( col.x * 255.f );
			sprite.color.g = static_cast<GLubyte>( col.y * 255.f );
			sprite.color.b = static_cast<GLubyte>( col.z * 255.f );
			sprite.color.a = static_cast<GLubyte>( col.w * 255.f );
		}

		auto& assetManager = MAIN_REGISTRY().GetAssetManager();

		std::string sSelectedTexture{ sprite.texture_name };
		ImGui::InlineLabel( "texture" );
		if (ImGui::BeginCombo("##texture", sSelectedTexture.c_str()))
		{
			for (const auto& sTextureName : assetManager.GetAssetKeyNames(SCION_UTIL::AssetType::TEXTURE))
			{
				if (ImGui::Selectable(sTextureName.c_str(), sTextureName == sSelectedTexture))
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
		if (ImGui::InputFloat("##width", &sprite.width, 8.f, 8.f))
		{
			sprite.width = std::clamp( sprite.width, 8.f, 1366.f );
			bChanged = true;
		}

		ImGui::InlineLabel( "height" );
		if ( ImGui::InputFloat( "##height", &sprite.height, 8.f, 8.f ) )
		{
			sprite.height = std::clamp( sprite.height, 8.f, 768.f );
			bChanged = true;
		}

		ImGui::InlineLabel( "layer" );
		if ( ImGui::InputInt( "##layer", &sprite.layer, 1, 1 ) )
		{
			sprite.layer = std::clamp( sprite.layer, 0, 10);
		}

		ImGui::InlineLabel( "Sprite Sheet Position" );
		ImGui::ColoredLabel( "x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		if (ImGui::InputInt("##start_x", &sprite.start_x, 1, 1))
		{
			sprite.start_x = std::clamp( sprite.start_x, 0, 32 );
			bChanged = true;
		}
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_RED );
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

	if (bChanged)
	{
		auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sprite.texture_name );
		if ( !pTexture )
			return;

		SCION_CORE::GenerateUVs(sprite, pTexture->GetWidth(), pTexture->GetHeight());
	}
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::AnimationComponent& animation )
{
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::BoxColliderComponent& boxCollider )
{
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::CircleColliderComponent& circleCollider )
{
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::PhysicsComponent& physics )
{
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::RigidBodyComponent& rigidbody )
{
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::TextComponent& textComponent )
{
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Identification& identification )
{
}

} // namespace SCION_EDITOR
