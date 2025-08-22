#include "DrawComponentUtils.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"
#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/Events/EventDispatcher.h"
#include "ScionUtilities/ScionUtilities.h"
#include "Rendering/Essentials/Texture.h"
#include "Physics/PhysicsUtilities.h"
#include "Logger/Logger.h"

#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/events/EditorEventTypes.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

using namespace SCION_UTIL;
using namespace SCION_PHYSICS;
using namespace SCION_CORE::ECS;

static constexpr std::string GetPhysicsBodyDescription( RigidBodyType eType )
{
	switch ( eType )
	{
	case RigidBodyType::STATIC: return "zero mass, zero velocity, may be manually moved";
	case RigidBodyType::KINEMATIC: return "zero mass, velocity set by user, moved by solver";
	case RigidBodyType::DYNAMIC: return "positive mass, velocity determined by forces, moved by solver";
	default: return "";
	}
}

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
		ImGui::ColoredLabel( "x##pos_x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		ImGui::InputFloat( "##position_x", &transform.position.x, 1.f, 10.f, "%.1f" );
		ImGui::SameLine();
		ImGui::ColoredLabel( "y##pos_y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		ImGui::InputFloat( "##position_y", &transform.position.y, 1.f, 10.f, "%.1f" );

		ImGui::InlineLabel( "scale" );
		ImGui::ColoredLabel( "x##scl_x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##scale_x", &transform.scale.x, 1.f, 1.f, "%.1f" ) )
		{
			transform.scale.x = std::clamp( transform.scale.x, 0.1f, 150.f );
			transform.bDirty = true;
		}
		ImGui::SameLine();
		ImGui::ColoredLabel( "y##scl_y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##scale_y", &transform.scale.y, 1.f, 1.f, "%.1f" ) )
		{
			transform.scale.y = std::clamp( transform.scale.y, 0.1f, 150.f );
			transform.bDirty = true;
		}

		ImGui::InlineLabel( "rotation" );
		if ( ImGui::InputFloat( "##rotation", &transform.rotation, 1.f, 1.f, "%.1f" ) )
		{
			transform.bDirty = true;
		}

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
					sprite.sTextureName = textureStr;
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

		std::string sSelectedTexture{ sprite.sTextureName };
		ImGui::InlineLabel( "texture" );
		ImGui::ItemToolTip( "The current active texture of the sprite to be drawn." );
		if ( ImGui::BeginCombo( "##texture", sSelectedTexture.c_str() ) )
		{
			for ( const auto& sTextureName : assetManager.GetAssetKeyNames( SCION_UTIL::AssetType::TEXTURE ) )
			{
				if ( ImGui::Selectable( sTextureName.c_str(), sTextureName == sSelectedTexture ) )
				{
					sSelectedTexture = sTextureName;
					sprite.sTextureName = sSelectedTexture;
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
			sprite.layer = std::clamp( sprite.layer, 0, 255 );
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

		ImGui::InlineLabel( "Iso Sorting" );
		ImGui::Checkbox( "##isoSorting", &sprite.bIsoMetric );
		ImGui::ItemToolTip( "If the scene is Isometric, the sprite should use iso sorting." );

		ImGui::TreePop();
		ImGui::PopItemWidth();
	}

	ImGui::PopID();

	if ( bChanged )
	{
		auto pTexture = MAIN_REGISTRY().GetAssetManager().GetTexture( sprite.sTextureName );
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
		// Should probably add a way to disable clamps
		if ( ImGui::InputFloat( "##offset_x", &boxCollider.offset.x, 1.f, 4.f ) )
		{
			boxCollider.offset.x = std::clamp( boxCollider.offset.x, -256.f, 256.f );
		}
		ImGui::SameLine();
		ImGui::ColoredLabel( "y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##offset_y", &boxCollider.offset.y, 1.f, 4.f ) )
		{
			boxCollider.offset.y = std::clamp( boxCollider.offset.y, -256.f, 256.f );
		}
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
		ImGui::ItemToolTip( "The radius of the circle component. This is clamped to [4 : 1366]" );
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
		ImGui::AddSpaces( 2 );

		PhysicsAttributes& physicsAttributes = physics.GetChangableAttributes();
		std::string sSelectedBodyType{ GetRigidBodyTypeString( physicsAttributes.eType ) };

		ImGui::InlineLabel( "body type" );
		ImGui::ItemToolTip( "The body type: static, kinematic, or dynamic." );
		if ( ImGui::BeginCombo( "##body_type", sSelectedBodyType.c_str() ) )
		{
			for ( const auto& [ eBodyType, sBodyStr ] : GetRigidBodyStringMap() )
			{
				if ( ImGui::Selectable( sBodyStr.c_str(), sBodyStr == sSelectedBodyType ) )
				{
					sSelectedBodyType = sBodyStr;
					physicsAttributes.eType = eBodyType;
				}

				ImGui::ItemToolTip( "{}", GetPhysicsBodyDescription( eBodyType ) );
			}

			ImGui::EndCombo();
		}

		std::string sSelectedCategoryType{
			GetFilterCategoryString( static_cast<FilterCategory>( physicsAttributes.filterCategory ) ) };

		ImGui::InlineLabel( "category type" );
		ImGui::ItemToolTip( "The collision category bits. Bodies will usually only use one category type." );
		if ( ImGui::BeginCombo( "##category_type", sSelectedCategoryType.c_str() ) )
		{
			for ( const auto& [ eCategoryType, sCategoryStr ] : GetFilterCategoryToStringMap() )
			{
				if ( ImGui::Selectable( sCategoryStr.c_str(), sCategoryStr == sSelectedCategoryType ) )
				{
					sSelectedCategoryType = sCategoryStr;
					physicsAttributes.filterCategory = static_cast<uint16_t>( eCategoryType );
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();
		ImGui::AddSpaces( 2 );

		std::string sSelectedMaskBit{  };
		FilterCategory eMaskCategory{ FilterCategory::NO_CATEGORY };

		ImGui::InlineLabel( "masks" );
		ImGui::ItemToolTip(
			"The collision mask bits. This states the categories that this shape would accept for collision." );
		if ( ImGui::BeginCombo( "##mask_bits", sSelectedMaskBit.c_str() ) )
		{
			for ( const auto& [ eCategoryType, sCategoryStr ] : GetFilterCategoryToStringMap() )
			{
				if ( ImGui::Selectable( sCategoryStr.c_str(), sCategoryStr == sSelectedMaskBit ) )
				{
					sSelectedMaskBit = sCategoryStr;
					eMaskCategory = eCategoryType;
				}
			}

			if ( !sSelectedMaskBit.empty() && eMaskCategory != FilterCategory::NO_CATEGORY )
			{
				uint16_t filterCat = static_cast<uint16_t>( eMaskCategory );
				if ( !( IsBitSet( physicsAttributes.filterMask, filterCat ) ) )
				{
					physicsAttributes.filterMask += filterCat;
					eMaskCategory = FilterCategory::NO_CATEGORY;
					sSelectedMaskBit.clear();
				}
				else
				{
					SCION_ERROR( "Masks already contain [{}]", sSelectedMaskBit );
				}
			}

			ImGui::EndCombo();
		}

		if ( physicsAttributes.filterMask > 0 )
		{
			ImGui::InlineLabel( "applied masks" );
			if ( ImGui::BeginListBox( "##applied_masks", ImVec2{ 0.f, 32.f } ) )
			{
				auto setMaskBits{ GetAllSetBits( physicsAttributes.filterMask ) };
				for ( auto mask : setMaskBits )
				{
					ImGui::Selectable( GetFilterCategoryString( static_cast<FilterCategory>( Bit( mask ) ) ).c_str() );
				}

				ImGui::EndListBox();
			}

			ImGui::SetCursorPosX( 128.f );
			if ( ImGui::Button( "clear masks" ) )
			{
				physicsAttributes.filterMask = 0;
				eMaskCategory = FilterCategory::NO_CATEGORY;
				sSelectedMaskBit.clear();
			}
		}

		ImGui::AddSpaces( 2 );
		ImGui::Separator();
		ImGui::AddSpaces( 2 );

		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "density" );
		ImGui::ItemToolTip( "The density, usually in kg/m^2." );
		ImGui::InputFloat( "##density", &physicsAttributes.density, 1.f, 1.f, "%.1f" );

		ImGui::InlineLabel( "friction" );
		ImGui::ItemToolTip( "The Coulomb (dry) friction coefficient, usually in the range [0,1]." );
		if ( ImGui::InputFloat( "##friction", &physicsAttributes.friction, 0.1f, 0.1f, "%.1f" ) )
		{
			physicsAttributes.friction = std::clamp( physicsAttributes.friction, 0.f, 1.f );
		}

		ImGui::InlineLabel( "restitution" );
		ImGui::ItemToolTip( "The restitution (bounce) usually in the range [0,1]." );
		if ( ImGui::InputFloat( "##restitution", &physicsAttributes.restitution, 0.1f, 0.1f, "%.1f" ) )
		{
			physicsAttributes.restitution = std::clamp( physicsAttributes.restitution, 0.f, 1.f );
		}

		ImGui::InlineLabel( "gravityScale" );
		ImGui::ItemToolTip( "Scale the gravity applied to this body. Non-dimensional." );
		ImGui::InputFloat( "##gravityScale", &physicsAttributes.gravityScale, 1.f, 1.f, "%.1f" );

		ImGui::AddSpaces( 2 );
		ImGui::Separator();
		ImGui::AddSpaces( 2 );

		ImGui::InlineLabel( "is a box?" );
		ImGui::ItemToolTip( "If the entity is a box, the box collider is used in construction for the size." );
		if ( ImGui::Checkbox( "##boxShape", &physicsAttributes.bBoxShape ) )
		{
			if ( physicsAttributes.bCircle )
				physicsAttributes.bCircle = false;
		}

		if ( physicsAttributes.bBoxShape && !physicsAttributes.bCircle )
		{
			ImGui::SameLine( 0, 32.f );
			ImGui::TextColored( ImVec4{ 1.f, 1.f, 0.f, 1.f }, "Must have a Box Collider Component." );
		}

		ImGui::InlineLabel( "is a circle" );
		ImGui::ItemToolTip( "If the entity is a circle, the circle collider is used in construction for the size." );
		if ( ImGui::Checkbox( "##circle", &physicsAttributes.bCircle ) )
		{
			if ( physicsAttributes.bBoxShape )
				physicsAttributes.bBoxShape = false;
		}

		if ( !physicsAttributes.bBoxShape && physicsAttributes.bCircle )
		{
			ImGui::SameLine( 0, 32.f );
			ImGui::TextColored( ImVec4{ 1.f, 1.f, 0.f, 1.f }, "Must have a Circle Collider Component." );
		}

		ImGui::AddSpaces( 2 );
		ImGui::Separator();

		ImGui::InlineLabel( "fixed rotation" );
		ImGui::ItemToolTip( "Should the body be prevented from rotating?" );
		ImGui::Checkbox( "##fixedRotation", &physicsAttributes.bFixedRotation );

		ImGui::InlineLabel( "sensor" );
		ImGui::ItemToolTip( "A sensor shape generates overlap events but never generates a collision response.\n"
							"Sensors do not collide with other sensors and do not have continuous collision" );
		ImGui::Checkbox( "##sensor", &physicsAttributes.bIsSensor );

		ImGui::InlineLabel( "bullet" );
		ImGui::ItemToolTip( "Treat this body as high speed object that performs continuous collision detection against "
							"dynamic and kinematic bodies, but not other bullet bodies.\n"
							"Warning - Bullets should be used sparingly. They are not a solution for general "
							"dynamic-versus-dynamic continuous collision. They may interfere with joint constraints." );
		ImGui::Checkbox( "##bullet", &physicsAttributes.bIsBullet );

		ImGui::SeparatorText( "Physics Object Data" );
		ImGui::AddSpaces( 2 );

		ImGui::PopItemWidth();
		auto& objectData = physicsAttributes.objectData;

		std::string sTagBuffer{ objectData.tag };
		ImGui::InlineLabel( "tag" );
		if ( ImGui::InputText(
				 "##_tag", sTagBuffer.data(), sizeof( char ) * 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			objectData.tag = std::string{ sTagBuffer.data() };
		}

		std::string sGroupBuffer{ objectData.group };
		ImGui::InlineLabel( "group" );
		if ( ImGui::InputText(
				 "##_group", sGroupBuffer.data(), sizeof( char ) * 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			objectData.group = std::string{ sGroupBuffer.data() };
		}

		ImGui::InlineLabel( "collider?" );
		ImGui::Checkbox( "##objectDataCollider", &objectData.bCollider );

		ImGui::InlineLabel( "trigger?" );
		ImGui::Checkbox( "##objectDataTrigger", &objectData.bTrigger );

		ImGui::InlineLabel( "friendly?" );
		ImGui::Checkbox( "##objectDataFriendly", &objectData.bIsFriendly );

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
				 "##_textStr", sTextBuffer.data(), sizeof( char ) * 1024, 0 /*ImGuiInputTextFlags_EnterReturnsTrue*/ ) )
		{
			textComponent.sTextStr = std::string{ sTextBuffer.data() };
			textComponent.bDirty = true;
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
					textComponent.bDirty = true;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( "padding" );
		if ( ImGui::InputInt( "##padding", &textComponent.padding, 0, 0 ) )
		{
			textComponent.bDirty = true;
		}

		ImGui::InlineLabel( "wrap" );
		if ( ImGui::InputFloat( "##textWrap", &textComponent.wrap, 0.f, 0.f ) )
		{
			textComponent.bDirty = true;
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

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity,
											 SCION_CORE::ECS::TransformComponent& transform )
{
	ImGui::SeparatorText( "Transform" );
	ImGui::PushID( entt::type_hash<TransformComponent>::value() );
	if ( ImGui::TreeNodeEx( "##TransformTree", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		const auto& relations = entity.GetComponent<Relationship>();
		bool bHasParent{ relations.parent != entt::null };
		bool bPositionChanged{ false };

		ImGui::PushItemWidth( 120.f );
		ImGui::InlineLabel( bHasParent ? "relative pos" : "position" );
		ImGui::ItemToolTip( bHasParent ? "Game object has a parent. "
										 "This is the relative position based on the parent's position."
									   : "World or absolute position of the game object." );

		ImGui::ColoredLabel( "x##pos_x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		if ( ImGui::InputFloat(
				 "##position_x", bHasParent ? &transform.localPosition.x : &transform.position.x, 1.f, 10.f, "%.1f" ) )
		{
			bPositionChanged = true;
		}

		ImGui::SameLine();
		ImGui::ColoredLabel( "y##pos_y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if ( ImGui::InputFloat(
				 "##position_y", bHasParent ? &transform.localPosition.y : &transform.position.y, 1.f, 10.f, "%.1f" ) )
		{
			bPositionChanged = true;
		}

		if ( bPositionChanged )
		{
			entity.UpdateTransform();
			// TODO: Post an event!
		}

		ImGui::InlineLabel( "scale" );
		ImGui::ColoredLabel( "x##scl_x", LABEL_SINGLE_SIZE, LABEL_RED );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##scale_x", &transform.scale.x, 1.f, 1.f, "%.1f" ) )
		{
			transform.scale.x = std::clamp( transform.scale.x, 0.1f, 150.f );
			transform.bDirty = true;
		}
		ImGui::SameLine();
		ImGui::ColoredLabel( "y##scl_y", LABEL_SINGLE_SIZE, LABEL_GREEN );
		ImGui::SameLine();
		if ( ImGui::InputFloat( "##scale_y", &transform.scale.y, 1.f, 1.f, "%.1f" ) )
		{
			transform.scale.y = std::clamp( transform.scale.y, 0.1f, 150.f );
			transform.bDirty = true;
		}

		ImGui::InlineLabel( "rotation" );
		if ( ImGui::InputFloat( "##rotation", &transform.rotation, 1.f, 1.f, "%.1f" ) )
		{
			transform.bDirty = true;
		}

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity, SCION_CORE::ECS::SpriteComponent& sprite )
{
	DrawImGuiComponent( sprite );
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity,
											 SCION_CORE::ECS::AnimationComponent& animation )
{
	DrawImGuiComponent( animation );
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity,
											 SCION_CORE::ECS::BoxColliderComponent& boxCollider )
{
	DrawImGuiComponent( boxCollider );
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity,
											 SCION_CORE::ECS::CircleColliderComponent& circleCollider )
{
	DrawImGuiComponent( circleCollider );
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity,
											 SCION_CORE::ECS::PhysicsComponent& physics )
{
	DrawImGuiComponent( physics );
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity,
											 SCION_CORE::ECS::RigidBodyComponent& rigidbody )
{
	DrawImGuiComponent( rigidbody );
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity,
											 SCION_CORE::ECS::TextComponent& textComponent )
{
	DrawImGuiComponent( textComponent );
}

void DrawComponentsUtil::DrawImGuiComponent( SCION_CORE::ECS::Entity& entity,
											 SCION_CORE::ECS::Identification& identification )
{

	ImGui::SeparatorText( "Identificaton" );
	ImGui::PushID( entt::type_hash<Identification>::value() );
	if ( ImGui::TreeNodeEx( "", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		std::string sError{  };
		std::string sNameBuffer{ identification.name };
		bool bNameError{ false };

		ImGui::InlineLabel( "name" );
		if ( ImGui::InputText(
				 "##_name", sNameBuffer.data(), sizeof( char ) * 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			std::string sBufferStr{ sNameBuffer.data() };
			if ( !sBufferStr.empty() && !SCENE_MANAGER().CheckTagName( sBufferStr ) )
			{
				std::string sOldName{ identification.name };
				identification.name = std::string{ sNameBuffer.data() };
				EVENT_DISPATCHER().EmitEvent( Events::NameChangeEvent{
					.sOldName = sOldName, .sNewName = identification.name, .pEntity = &entity } );
			}
		}

		// We need to display an error if the name is already in the scene or empty.
		// Entity names need to be unique.
		if ( ImGui::IsItemActive() )
		{
			std::string sBufferStr{ sNameBuffer.data() };
			if ( sBufferStr.empty() )
			{
				sError = "Name cannot be empty.";
			}
			else if ( SCENE_MANAGER().CheckTagName( sBufferStr ) )
			{
				sError = fmt::format( "{} already exists!", sBufferStr );
			}

			if ( !sError.empty() )
			{
				ImGui::TextColored( ImVec4{ 1.f, 0.f, 0.f, 1.f }, sError.c_str() );
			}
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
