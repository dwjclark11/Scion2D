#include "SceneHierarchyDisplay.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/utilities/ImGuiUtils.h"

#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/MetaUtilities.h"
#include "Core/ECS/Components/AllComponents.h"

#include <imgui.h>

using namespace SCION_CORE::ECS;
using namespace entt::literals;

namespace SCION_EDITOR
{

auto create_entity = [ ]( SCION_EDITOR::SceneObject& currentScene ) {
	Entity newEntity{ currentScene.GetRegistry(), "GameObject", "" };
	newEntity.AddComponent<TransformComponent>();
};

bool SceneHierarchyDisplay::OpenTreeNode( SCION_CORE::ECS::Entity& entity )
{
	const auto& name = entity.GetName();

	ImGui::PushID( static_cast<int32_t>( entity.GetEntity() ) );

	ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding |
								   ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	// If the selected entity is the current node, highlight the selected node
	if ( m_pSelectedEntity && m_pSelectedEntity->GetEntity() == entity.GetEntity() )
		nodeFlags |= ImGuiTreeNodeFlags_Selected;

	bool bTreeNodeOpen{ false };

	bTreeNodeOpen = ImGui::TreeNodeEx( name.c_str(), nodeFlags );

	if ( ImGui::IsItemClicked() )
	{
		m_pSelectedEntity =
			std::make_shared<Entity>( SCENE_MANAGER().GetCurrentScene()->GetRegistry(), entity.GetEntity() );
	}

	ImGui::PopID();

	return bTreeNodeOpen;
}

void SceneHierarchyDisplay::AddComponent( SCION_CORE::ECS::Entity& entity, bool* bAddComponent )
{
	if ( !bAddComponent )
		return;

	if ( *bAddComponent )
		ImGui::OpenPopup( "Add Component" );

	if ( ImGui::BeginPopupModal( "Add Component" ) )
	{
		auto& registry = entity.GetRegistry();

		// Get all of the reflected types from entt::meta
		std::map<entt::id_type, std::string> componentMap;

		for ( auto&& [ id, type ] : entt::resolve() )
		{
			const auto& info = type.info();
			auto pos = info.name().find_last_of( ':' ) + 1;
			auto name = info.name().substr( pos );
			componentMap[ id ] = std::string{ name };
		}

		static std::string componentStr{ "" };
		static std::string componentStrPrev{ "" };
		static entt::id_type id_type{ 0 };
		static bool bError{ false };

		if ( ImGui::BeginCombo( "Choose Component", componentStr.c_str() ) )
		{
			for ( const auto& [ id, name ] : componentMap )
			{
				if ( ImGui::Selectable( name.c_str(), name == componentStr ) )
				{
					componentStr = name;
					id_type = id;
				}
			}
			ImGui::EndCombo();
		}

		if ( componentStr != componentStrPrev )
		{
			componentStrPrev = componentStr;
			bError = false;
		}

		if ( bError )
		{
			ImGui::TextColored(
				ImVec4{ 1.f, 0.f, 0.f, 1.f },
				fmt::format( "Game Object already has [{}] - Please make another selection.", componentStr ).c_str() );
		}

		if ( ImGui::Button( "Ok" ) && !bError )
		{
			if ( !componentStr.empty() )
			{
				for ( auto&& [ id, storage ] : registry.storage() )
				{
					if ( id != id_type )
						continue;

					if ( storage.contains( entity.GetEntity() ) )
					{
						bError = true;
						SCION_ERROR( "Entity already has component [{}]", componentStr );
						break;
					}

					// The entity does not have that component yet, proceed.
					break;
				}
			}

			if ( !bError && id_type != 0 )
			{
				auto&& storage = registry.storage( id_type );
				if ( !storage )
				{
					const auto addComponent =
						SCION_CORE::Utils::InvokeMetaFunction( id_type, "add_component_default"_hs, entity );

					if ( addComponent )
					{
						*bAddComponent = false;
						ImGui::CloseCurrentPopup();
					}
					else
					{
						// This should probably fail/throw
						SCION_ASSERT( addComponent && "Failed to add component." );
						*bAddComponent = false;
						bError = true;
						ImGui::CloseCurrentPopup();
					}
				}
				else 
				{
					storage->push( entity.GetEntity() );
					*bAddComponent = false;
					ImGui::CloseCurrentPopup();
				}
			}
		}

		ImGui::SameLine();
		if ( ImGui::Button( "Cancel" ) )
		{
			bError = false;
			*bAddComponent = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void SceneHierarchyDisplay::DrawGameObjectDetails()
{
	if ( !ImGui::Begin( "Object Details" ) )
	{
		ImGui::End();
		return;
	}

	if ( ImGui::BeginPopupContextWindow() )
	{
		if ( ImGui::Selectable( "Add Component" ) )
			m_bAddComponent = true;

		ImGui::EndPopup();
	}

	if ( m_pSelectedEntity && m_bAddComponent )
		AddComponent( *m_pSelectedEntity, &m_bAddComponent );

	if ( m_pSelectedEntity )
		DrawEntityComponents();

	ImGui::End();
}

void SceneHierarchyDisplay::DrawEntityComponents()
{
	if ( !m_pSelectedEntity )
		return;

	auto& registry = m_pSelectedEntity->GetRegistry();

	for ( const auto&& [ id, storage ] : registry.storage() )
	{
		if ( !storage.contains( m_pSelectedEntity->GetEntity() ) )
			continue;

		if ( id == entt::type_hash<TileComponent>::value() )
			continue;

		const auto drawInfo =
			SCION_CORE::Utils::InvokeMetaFunction( id, "DrawEntityComponentInfo"_hs, *m_pSelectedEntity );

		if ( drawInfo )
		{
			ImGui::Spacing();
			ImGui::PushID( id );
			if ( ImGui::Button( "remove" ) )
			{
				storage.remove( m_pSelectedEntity->GetEntity() );
			}
			ImGui::PopID();
		}

		ImGui::Spacing();
		ImGui::Separator();
	}
}

SceneHierarchyDisplay::SceneHierarchyDisplay()
{
}

SceneHierarchyDisplay::~SceneHierarchyDisplay()
{
}

void SceneHierarchyDisplay::Update()
{
}

void SceneHierarchyDisplay::Draw()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	if ( !ImGui::Begin( "Scene Hierarchy" ) || !pCurrentScene )
	{
		ImGui::End();
		return;
	}

	if ( ImGui::BeginPopupContextWindow() )
	{
		if ( ImGui::Selectable( "Add New Game Object" ) )
			create_entity( *pCurrentScene );

		ImGui::EndPopup();
	}

	ImGui::Separator();
	ImGui::AddSpaces( 1 );
	ImGui::InlineLabel( ICON_FA_SEARCH, 32.f );
	m_TextFilter.Draw( "##search_filter" );
	ImGui::AddSpaces( 1 );
	ImGui::Separator();

	auto& registry = pCurrentScene->GetRegistry();
	auto sceneEntities = registry.GetRegistry().view<entt::entity>( entt::exclude<TileComponent, ScriptComponent> );

	for ( auto entity : sceneEntities )
	{
		Entity ent{ registry, entity };
		if ( !m_TextFilter.PassFilter(ent.GetName().c_str() ))
			continue;

		if ( OpenTreeNode( ent ) )
			ImGui::TreePop();
	}

	ImGui::End();

	DrawGameObjectDetails();
}
} // namespace SCION_EDITOR
