#include "TilemapDisplay.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/Resources/AssetManager.h"

#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/RenderShapeSystem.h"
#include "Core/Systems/RenderPickingSystem.h"
#include "Core/Systems/AnimationSystem.h"

#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/CoreUtilities/Prefab.h"

#include "Core/Events/EventDispatcher.h"
#include "Core/Events/EngineEventTypes.h"

#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/Renderer.h"
#include "Rendering/Essentials/PickingTexture.h"

#include "editor/systems/GridSystem.h"
#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/imgui/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

#include "editor/tools/ToolManager.h"
#include "editor/tools/ToolAccessories.h"
#include "editor/tools/CreateTileTool.h"

#include "editor/tools/gizmos/Gizmo.h"

#include "editor/commands/CommandManager.h"

#include "editor/events/EditorEventTypes.h"

#include "Core/Scripting/InputManager.h"
#include "Windowing/Inputs/Mouse.h"

#include "Logger/Logger.h"
#include <imgui.h>

#ifdef __linux
#include <signal.h>
#endif

using namespace SCION_CORE::ECS;
using namespace SCION_CORE::Systems;
using namespace SCION_RENDERING;

namespace SCION_EDITOR
{
void TilemapDisplay::RenderTilemap()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	auto& mainRegistry = MAIN_REGISTRY();
	auto& editorFramebuffers = mainRegistry.GetContext<std::shared_ptr<EditorFramebuffers>>();
	auto& renderer = mainRegistry.GetContext<std::shared_ptr<SCION_RENDERING::Renderer>>();

	auto& renderSystem = mainRegistry.GetRenderSystem();
	auto& renderUISystem = mainRegistry.GetRenderUISystem();
	auto& renderShapeSystem = mainRegistry.GetRenderShapeSystem();

	auto pActiveGizmo = TOOL_MANAGER().GetActiveGizmo();
	auto& mouse = INPUT_MANAGER().GetMouse();

	if ( pActiveGizmo && pActiveGizmo->IsOverTilemapWindow() && !pActiveGizmo->OverGizmo() &&
		 !ImGui::GetDragDropPayload() && mouse.IsBtnJustPressed( SCION_MOUSE_LEFT ) )
	{
		auto& renderPickingSystem = mainRegistry.GetContext<std::shared_ptr<RenderPickingSystem>>();
		// Handle the picking texture/system
		if ( renderPickingSystem && pCurrentScene )
		{
			auto& pPickingTexture = mainRegistry.GetContext<std::shared_ptr<PickingTexture>>();
			if ( pPickingTexture )
			{
				renderer->SetCapability( Renderer::GLCapability::BLEND, false );
				pPickingTexture->Bind();
				renderer->SetViewport( 0, 0, pPickingTexture->GetWidth(), pPickingTexture->GetHeight() );
				renderer->SetClearColor( 0.f, 0.f, 0.f, 0.f );
				renderer->ClearBuffers( true, true );

				renderPickingSystem->Update( pCurrentScene->GetRegistry(), *m_pTilemapCam );

				const auto& pos = pActiveGizmo->GetMouseScreenCoords();
				auto id = static_cast<entt::entity>(
					pPickingTexture->ReadPixel( static_cast<int>( pos.x ), static_cast<int>( pos.y ) ) );

				if ( !pCurrentScene->GetRegistry().IsValid( static_cast<entt::entity>( id ) ) )
				{
					id = entt::null;
				}
				else
				{
					SCION_CORE::ECS::Entity checkedEntity{ pCurrentScene->GetRegistry(),
														   static_cast<entt::entity>( id ) };
					if ( checkedEntity.HasComponent<SCION_CORE::ECS::TileComponent>() )
					{
						id = entt::null;
					}
				}

				SCENE_MANAGER().GetToolManager().SetSelectedEntity( id );
			}

			pPickingTexture->Unbind();
			pPickingTexture->CheckResize();
			renderer->SetCapability( Renderer::GLCapability::BLEND, true );
		}
	}

	const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::TILEMAP ];

	fb->Bind();
	renderer->SetViewport( 0, 0, fb->Width(), fb->Height() );
	renderer->SetClearColor( 0.f, 0.f, 0.f, 1.f );
	renderer->ClearBuffers( true, true, false );

	if ( !pCurrentScene )
	{
		fb->Unbind();
		return;
	}

	auto& gridSystem = mainRegistry.GetContext<std::shared_ptr<GridSystem>>();
	gridSystem->Update( *pCurrentScene, *m_pTilemapCam );

	renderSystem.Update( pCurrentScene->GetRegistry(), *m_pTilemapCam, pCurrentScene->GetLayerParams() );

	if ( CORE_GLOBALS().RenderCollidersEnabled() )
	{
		renderShapeSystem.Update( pCurrentScene->GetRegistry(), *m_pTilemapCam );
	}

	renderUISystem.Update( pCurrentScene->GetRegistry() );

	auto pActiveTool = TOOL_MANAGER().GetActiveTool();
	if ( pActiveTool )
		pActiveTool->Draw();

	if ( pActiveGizmo )
		pActiveGizmo->Draw( renderUISystem.GetCamera() );

	fb->Unbind();
	fb->CheckResize();

	renderer->SetClearColor( 0.f, 0.f, 0.f, 1.f );
	renderer->ClearBuffers( true, true, false );
}

void TilemapDisplay::LoadNewScene()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
	if ( !pCurrentScene )
		return;

	auto& toolManager = TOOL_MANAGER();

	if ( !toolManager.SetupTools( pCurrentScene, m_pTilemapCam.get() ) )
	{
		SCION_ASSERT( false && "This should work!!" );
#ifdef _WIN32
		__debugbreak();
#elif __linux
		raise( SIGTRAP );
#endif
	}

	if ( !SCENE_MANAGER().GetCurrentTileset().empty() )
		toolManager.SetToolsCurrentTileset( SCENE_MANAGER().GetCurrentTileset() );
}

void TilemapDisplay::PanZoomCamera( const glm::vec2& mousePos )
{
	if ( !m_pTilemapCam )
		return;

	auto& mouse = INPUT_MANAGER().GetMouse();

	if ( !mouse.IsBtnJustPressed( SCION_MOUSE_MIDDLE ) && !mouse.IsBtnPressed( SCION_MOUSE_MIDDLE ) &&
		 mouse.GetMouseWheelY() == 0 )
	{
		return;
	}

	static glm::vec2 startPosition{ 0.f };
	auto screenOffset = m_pTilemapCam->GetScreenOffset();
	bool bOffsetChanged{ false }, bScaledChanged{ false };

	if ( mouse.IsBtnJustPressed( SCION_MOUSE_MIDDLE ) )
	{
		startPosition = mousePos;
	}

	if ( mouse.IsBtnPressed( SCION_MOUSE_MIDDLE ) )
	{
		screenOffset += ( mousePos - startPosition );
		bOffsetChanged = true;
	}

	glm::vec2 currentWorldPos = m_pTilemapCam->ScreenCoordsToWorld( mousePos );
	float scale = m_pTilemapCam->GetScale();

	if ( mouse.GetMouseWheelY() == 1 )
	{
		scale += 0.2f;
		bScaledChanged = true;
		bOffsetChanged = true;
	}
	else if ( mouse.GetMouseWheelY() == -1 )
	{
		scale -= 0.2f;
		bScaledChanged = true;
		bOffsetChanged = true;
	}

	if ( bScaledChanged )
	{
		scale = std::clamp( scale, 1.f, 10.f );
		m_pTilemapCam->SetScale( scale );
	}

	if ( bOffsetChanged )
	{
		glm::vec2 afterMovePos = m_pTilemapCam->ScreenCoordsToWorld( mousePos );
		screenOffset += ( afterMovePos - currentWorldPos );
		m_pTilemapCam->SetScreenOffset( screenOffset );
	}

	startPosition = mousePos;
}

void TilemapDisplay::HandleKeyPressedEvent( const SCION_CORE::Events::KeyEvent& keyEvent )
{
	if ( !m_bWindowActive || keyEvent.eType == SCION_CORE::Events::EKeyEventType::Released )
		return;

	// No need to change the tools if there is no scene loaded.
	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	if ( !pCurrentScene )
		return;

	if ( keyEvent.key == SCION_KEY_W )
	{
		TOOL_MANAGER().SetGizmoActive( EGizmoType::TRANSLATE );
	}
	else if ( keyEvent.key == SCION_KEY_E )
	{
		TOOL_MANAGER().SetGizmoActive( EGizmoType::SCALE );
	}
	else if ( keyEvent.key == SCION_KEY_R )
	{
		TOOL_MANAGER().SetGizmoActive( EGizmoType::ROTATE );
	}
	else if ( keyEvent.key == SCION_KEY_T )
	{
		TOOL_MANAGER().SetToolActive( EToolType::CREATE_TILE );
	}
	else if ( keyEvent.key == SCION_KEY_Y )
	{
		// IsoGrid scenes are not currently supported for rect tool.
		if ( pCurrentScene->GetMapType() == SCION_CORE::EMapType::Grid )
		{
			TOOL_MANAGER().SetToolActive( EToolType::RECT_FILL_TILE );
		}
	}
}

void TilemapDisplay::AddPrefabbedEntityToScene( const SCION_CORE::PrefabbedEntity& prefabbed )
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentSceneObject();
	if ( !pCurrentScene )
		return;

	int count{ 1 };
	std::string sTag{ prefabbed.id->name };
	while ( pCurrentScene->CheckTagName( sTag ) )
	{
		sTag = prefabbed.id->name + std::to_string( count );
		++count;
	}

	SCION_CORE::ECS::Entity newEnt{ pCurrentScene->GetRegistry(), sTag, prefabbed.id->group };

	newEnt.AddComponent<TransformComponent>( prefabbed.transform );
	if ( prefabbed.sprite )
	{
		newEnt.AddComponent<SpriteComponent>( prefabbed.sprite.value() );
	}

	if ( prefabbed.animation )
	{
		newEnt.AddComponent<AnimationComponent>( prefabbed.animation.value() );
	}

	if ( prefabbed.boxCollider )
	{
		newEnt.AddComponent<BoxColliderComponent>( prefabbed.boxCollider.value() );
	}

	if ( prefabbed.circleCollider )
	{
		newEnt.AddComponent<CircleColliderComponent>( prefabbed.circleCollider.value() );
	}

	if ( prefabbed.textComp )
	{
		newEnt.AddComponent<TextComponent>( prefabbed.textComp.value() );
	}

	if ( prefabbed.physics )
	{
		newEnt.AddComponent<PhysicsComponent>( prefabbed.physics.value() );
	}

	pCurrentScene->AddGameObjectByTag( sTag, newEnt.GetEntity() );
}

void TilemapDisplay::DrawToolbar()
{
	ImGui::Separator();

	ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1.f );
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, { 0.f, 0.f } );

	auto& commandManager = COMMAND_MANAGER();
	if ( commandManager.UndoEmpty() )
	{
		ImGui::DisabledButton( ICON_FA_UNDO, TOOL_BUTTON_SIZE, "Undo [CTRL + Z] - Nothing to undo." );
	}
	else
	{
		if ( ImGui::Button( ICON_FA_UNDO, TOOL_BUTTON_SIZE ) )
		{
			commandManager.Undo();
		}
	}
	ImGui::ItemToolTip( "Undo [CTRL + Z]" );

	ImGui::SameLine();

	if ( commandManager.RedoEmpty() )
	{
		ImGui::DisabledButton( ICON_FA_REDO, TOOL_BUTTON_SIZE, "Redo [CTRL + SHIFT + Z] - Nothing to redo." );
	}
	else
	{
		if ( ImGui::Button( ICON_FA_REDO, TOOL_BUTTON_SIZE ) )
		{
			commandManager.Redo();
		}
	}
	ImGui::ItemToolTip( "Redo [CTRL + SHIFT + Z]" );

	ImGui::SameLine( 0.f, 32.f );

	auto& toolManager = TOOL_MANAGER();
	const EToolType eActiveToolType = toolManager.GetActiveToolType();
	const EGizmoType eActiveGizmoType = toolManager.GetActiveGizmoType();

	ImGui::DisabledButton( ICON_FA_TOOLS "##1", TOOL_BUTTON_SIZE );

	ImGui::SameLine();

	if ( eActiveGizmoType == EGizmoType::TRANSLATE )
	{
		ImGui::ActiveButton( ICON_FA_ARROWS_ALT, TOOL_BUTTON_SIZE );
	}
	else
	{
		if ( ImGui::Button( ICON_FA_ARROWS_ALT, TOOL_BUTTON_SIZE ) )
		{
			toolManager.SetGizmoActive( EGizmoType::TRANSLATE );
		}
	}

	ImGui::ItemToolTip( "Translate [W] - Translate/Move game objects" );

	ImGui::SameLine();

	if ( eActiveGizmoType == EGizmoType::SCALE )
	{
		ImGui::ActiveButton( ICON_FA_EXPAND, TOOL_BUTTON_SIZE );
	}
	else
	{
		if ( ImGui::Button( ICON_FA_EXPAND, TOOL_BUTTON_SIZE ) )
		{
			toolManager.SetGizmoActive( EGizmoType::SCALE );
		}
	}

	ImGui::ItemToolTip( "Scale [E] - Scale game objects" );

	ImGui::SameLine();

	if ( eActiveGizmoType == EGizmoType::ROTATE )
	{
		ImGui::ActiveButton( ICON_FA_CIRCLE_NOTCH, TOOL_BUTTON_SIZE );
	}
	else
	{
		if ( ImGui::Button( ICON_FA_CIRCLE_NOTCH, TOOL_BUTTON_SIZE ) )
		{
			toolManager.SetGizmoActive( EGizmoType::ROTATE );
		}
	}

	ImGui::ItemToolTip( "Rotate [R] - Rotates game object" );

	ImGui::SameLine();

	if ( eActiveToolType == EToolType::CREATE_TILE )
	{
		ImGui::ActiveButton( ICON_FA_STAMP, TOOL_BUTTON_SIZE );
	}
	else
	{
		if ( ImGui::Button( ICON_FA_STAMP, TOOL_BUTTON_SIZE ) )
		{
			toolManager.SetToolActive( EToolType::CREATE_TILE );
		}
	}

	ImGui::ItemToolTip( "Create Tile [T] - Creates a single tile." );

	ImGui::SameLine();

	bool bIsoScene{ false };
	if ( auto pCurrentScene = SCENE_MANAGER().GetCurrentScene() )
	{
		bIsoScene = pCurrentScene->GetMapType() == SCION_CORE::EMapType::IsoGrid;
	}

	if ( bIsoScene )
	{
		ImGui::DisabledButton(
			ICON_FA_CHESS_BOARD, TOOL_BUTTON_SIZE, "Rect Tile Tool [Y] - IsoMetric grids not currently supported." );
	}
	else
	{
		if ( eActiveToolType == EToolType::RECT_FILL_TILE )
		{
			ImGui::ActiveButton( ICON_FA_CHESS_BOARD, TOOL_BUTTON_SIZE );
		}
		else
		{
			if ( ImGui::Button( ICON_FA_CHESS_BOARD, TOOL_BUTTON_SIZE ) )
			{
				toolManager.SetToolActive( EToolType::RECT_FILL_TILE );
			}
		}

		ImGui::ItemToolTip( "Rect Tile Tool [Y] - Creates tiles inside of created rectangle." );
	}

	ImGui::SameLine();

	ImGui::DisabledButton( ICON_FA_TOOLS "##2", TOOL_BUTTON_SIZE );

	ImGui::PopStyleVar( 2 );

	ImGui::SameLine( 0.f, 16.f );

	if ( auto pActiveTool = TOOL_MANAGER().GetActiveTool() )
	{
		const auto& gridCoords = pActiveTool->GetGridCoords();
		ImGui::TextColored( ImVec4{ 0.7f, 1.f, 7.f, 1.f },
							fmt::format( "Grid Coords [ x = {}, y = {} ]", gridCoords.x, gridCoords.y ).c_str() );
		ImGui::SameLine( 0.f, 16.f );

		const auto& worldCoords = pActiveTool->GetMouseWorldCoords();
		ImGui::TextColored( ImVec4{ 0.7f, 0.7f, 1.f, 1.f },
							fmt::format( "World Coords [ x = {}, y = {} ]", worldCoords.x, worldCoords.y ).c_str() );
	}

	ImGui::Separator();
	ImGui::AddSpaces( 1 );
}

TilemapDisplay::TilemapDisplay()
	: m_pTilemapCam{ std::make_unique<SCION_RENDERING::Camera2D>() }
	, m_bWindowActive{ false }
{
	ADD_EVENT_HANDLER( SCION_CORE::Events::KeyEvent, &TilemapDisplay::HandleKeyPressedEvent, *this );
}

TilemapDisplay::~TilemapDisplay()
{
}

void TilemapDisplay::Draw()
{
	if ( !ImGui::Begin( ICON_FA_MAP " Tilemap Editor" ) )
	{
		ImGui::End();
		return;
	}

	DrawToolbar();
	RenderTilemap();

	auto& mainRegistry = MAIN_REGISTRY();

	if ( ImGui::BeginChild( "##tilemap", ImVec2{ 0, 0 }, false, ImGuiWindowFlags_NoScrollWithMouse ) )
	{
		m_bWindowActive = ImGui::IsWindowFocused();

		auto& editorFramebuffers = mainRegistry.GetContext<std::shared_ptr<EditorFramebuffers>>();
		const auto& fb = editorFramebuffers->mapFramebuffers[ FramebufferType::TILEMAP ];

		ImVec2 imageSize{ static_cast<float>( fb->Width() ), static_cast<float>( fb->Height() ) };
		ImVec2 windowSize{ ImGui::GetWindowSize() };

		float x = ( windowSize.x - imageSize.x ) * 0.5f;
		float y = ( windowSize.y - imageSize.y ) * 0.5f;

		ImGui::SetCursorPos( ImVec2{ x, y } );
		ImGuiIO io = ImGui::GetIO();
		auto relativePos = ImGui::GetCursorScreenPos();
		auto windowPos = ImGui::GetWindowPos();

		auto pActiveTool = SCENE_MANAGER().GetToolManager().GetActiveToolFromAbstract();
		if ( pActiveTool )
		{
			pActiveTool->SetRelativeCoords( glm::vec2{ relativePos.x, relativePos.y } );
			pActiveTool->SetCursorCoords( glm::vec2{ io.MousePos.x, io.MousePos.y } );
			pActiveTool->SetWindowPos( glm::vec2{ windowPos.x, windowPos.y } );
			pActiveTool->SetWindowSize( glm::vec2{ windowSize.x, windowSize.y } );

			pActiveTool->SetOverTilemapWindow( ImGui::IsWindowHovered() );
		}

		ImGui::Image( (ImTextureID)(intptr_t)fb->GetTextureID(), imageSize, ImVec2{ 0.f, 1.f }, ImVec2{ 1.f, 0.f } );

		// Accept Scene Drop Target
		if ( ImGui::BeginDragDropTarget() )
		{

			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( std::string{ DROP_SCENE_SRC }.c_str() ) )
			{
				SCENE_MANAGER().UnloadCurrentScene();
				SCENE_MANAGER().SetCurrentScene( std::string{ (const char*)payload->Data } );
				SCENE_MANAGER().LoadCurrentScene();
				LoadNewScene();
				m_pTilemapCam->Reset();
			}
			else if ( const ImGuiPayload* payload =
						  ImGui::AcceptDragDropPayload( std::string{ DROP_PREFAB_SRC }.c_str() ) )
			{
				if ( auto pPrefab = ASSET_MANAGER().GetPrefab( std::string{ (const char*)payload->Data } ) )
				{
					const auto& prefabbed = pPrefab->GetPrefabbedEntity();
					AddPrefabbedEntityToScene( prefabbed );
				}
			}

			ImGui::EndDragDropTarget();
		}

		// Check for resize based on the window size
		if ( fb->Width() != static_cast<int>( windowSize.x ) || fb->Height() != static_cast<int>( windowSize.y ) )
		{
			fb->Resize( static_cast<int>( windowSize.x ), static_cast<int>( windowSize.y ) );

			auto& pPickingTexture = mainRegistry.GetContext<std::shared_ptr<PickingTexture>>();
			if ( pPickingTexture )
			{
				pPickingTexture->Resize( static_cast<int>( windowSize.x ), static_cast<int>( windowSize.y ) );
			}

			m_pTilemapCam->Resize( static_cast<int>( windowSize.x ), static_cast<int>( windowSize.y ) );
		}

		ImGui::EndChild();
	}
	ImGui::End();
}

void TilemapDisplay::Update()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	if ( !pCurrentScene )
		return;

	auto pActiveTool = TOOL_MANAGER().GetActiveTool();
	if ( pActiveTool && pActiveTool->IsOverTilemapWindow() && !ImGui::GetDragDropPayload() )
	{
		PanZoomCamera( pActiveTool->GetMouseScreenCoords() );

		pActiveTool->Update( pCurrentScene->GetCanvas() );
		pActiveTool->Create();
	}

	auto pActiveGizmo = TOOL_MANAGER().GetActiveGizmo();
	if ( pActiveGizmo && pActiveGizmo->IsOverTilemapWindow() && !ImGui::GetDragDropPayload() )
	{
		PanZoomCamera( pActiveGizmo->GetMouseScreenCoords() );
		pActiveGizmo->Update( pCurrentScene->GetCanvas() );
	}

	if ( CORE_GLOBALS().AnimationRenderEnabled() )
	{
		auto& mainRegistry = MAIN_REGISTRY();
		auto& animationSystem = mainRegistry.GetAnimationSystem();
		animationSystem.Update( pCurrentScene->GetRegistry(), *m_pTilemapCam );
	}

	m_pTilemapCam->Update();

	auto& keyboard = INPUT_MANAGER().GetKeyboard();
	if ( keyboard.IsKeyPressed( SCION_KEY_LCTRL ) && keyboard.IsKeyJustPressed( SCION_KEY_Z ) )
	{
		if ( keyboard.IsKeyPressed( SCION_KEY_LSHIFT ) )
			COMMAND_MANAGER().Redo();
		else
			COMMAND_MANAGER().Undo();
	}
}

} // namespace SCION_EDITOR
