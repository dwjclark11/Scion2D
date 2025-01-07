#include "TilemapDisplay.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"

#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/RenderShapeSystem.h"
#include "Core/Systems/AnimationSystem.h"

#include "Core/CoreUtilities/CoreEngineData.h"

#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/Renderer.h"

#include "editor/systems/GridSystem.h"
#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/EditorUtilities.h"
#include "editor/utilities/ImGuiUtils.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

#include "editor/tools/ToolManager.h"
#include "editor/tools/ToolAccessories.h"
#include "editor/tools/CreateTileTool.h"

#include "editor/tools/gizmos/Gizmo.h"

#include "editor/commands/CommandManager.h"

#include "Core/Scripting/InputManager.h"
#include "Windowing/Inputs/Mouse.h"

#include "Logger/Logger.h"
#include <imgui.h>

#ifdef __linux
#include <signal.h>
#endif

using namespace SCION_CORE::Systems;

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
		pActiveGizmo->Draw();

	fb->Unbind();
	fb->CheckResize();

	renderer->SetClearColor( 0.f, 0.f, 0.f, 1.f );
	renderer->ClearBuffers( true, true, false );
}

void TilemapDisplay::LoadNewScene()
{
	auto pCurrentScene = SCENE_MANAGER().GetCurrentScene();
	if ( !pCurrentScene )
		return;

	auto& toolManager = TOOL_MANAGER();

	if ( !toolManager.SetupTools( pCurrentScene.get(), m_pTilemapCam.get() ) )
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

	scale = std::clamp( scale, 1.f, 10.f );

	if ( bScaledChanged )
		m_pTilemapCam->SetScale( scale );

	glm::vec2 afterMovePos = m_pTilemapCam->ScreenCoordsToWorld( mousePos );

	screenOffset += ( afterMovePos - currentWorldPos );

	if ( bOffsetChanged )
		m_pTilemapCam->SetScreenOffset( screenOffset );

	startPosition = mousePos;
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

	ImGui::DisabledButton( ICON_FA_TOOLS, TOOL_BUTTON_SIZE );

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
		// ImGui::ActiveButton( ICON_FA_CIRCLE_NOTCH, TOOL_BUTTON_SIZE );
		ImGui::DisabledButton(
			ICON_FA_CIRCLE_NOTCH, TOOL_BUTTON_SIZE, "Rotate [R] - Rotates game object - Currently Unavailable." );
	}
	else
	{
		/*	if ( ImGui::Button( ICON_FA_CIRCLE_NOTCH, TOOL_BUTTON_SIZE ) )
			{
				toolManager.SetGizmoActive( EGizmoType::ROTATE);
			}*/
		ImGui::DisabledButton(
			ICON_FA_CIRCLE_NOTCH, TOOL_BUTTON_SIZE, "Rotate [R] - Rotates game object - Currently Unavailable." );
	}

	// ImGui::ItemToolTip( "Rotate [R] - Rotates game object" );

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

	ImGui::SameLine();

	ImGui::DisabledButton( ICON_FA_TOOLS, TOOL_BUTTON_SIZE );

	ImGui::PopStyleVar( 2 );
	ImGui::Separator();
	ImGui::AddSpaces( 1 );
}

TilemapDisplay::TilemapDisplay()
	: m_pTilemapCam{ std::make_unique<SCION_RENDERING::Camera2D>() }
{
}

TilemapDisplay::~TilemapDisplay()
{
}

void TilemapDisplay::Draw()
{
	if ( !ImGui::Begin( "Tilemap Editor" ) )
	{
		ImGui::End();
		return;
	}

	DrawToolbar();
	RenderTilemap();

	auto& mainRegistry = MAIN_REGISTRY();

	if ( ImGui::BeginChild( "##tilemap", ImVec2{ 0, 0 }, false, ImGuiWindowFlags_NoScrollWithMouse ) )
	{
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
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( DROP_SCENE_SRC );
			if ( payload )
			{
				SCENE_MANAGER().SetCurrentScene( std::string{ (const char*)payload->Data } );
				LoadNewScene();
				m_pTilemapCam->Reset();
			}

			ImGui::EndDragDropTarget();
		}

		// Check for resize based on the window size
		if ( fb->Width() != static_cast<int>( windowSize.x ) || fb->Height() != static_cast<int>( windowSize.y ) )
		{
			fb->Resize( static_cast<int>( windowSize.x ), static_cast<int>( windowSize.y ) );
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

	auto& mainRegistry = MAIN_REGISTRY();
	auto& animationSystem = mainRegistry.GetAnimationSystem();
	animationSystem.Update( pCurrentScene->GetRegistry(), *m_pTilemapCam );

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
