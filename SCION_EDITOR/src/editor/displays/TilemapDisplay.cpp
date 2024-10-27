#include "TilemapDisplay.h"
#include "Core/ECS/MainRegistry.h"
#include "Core/Resources/AssetManager.h"

#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/RenderUISystem.h"
#include "Core/Systems/RenderShapeSystem.h"

#include "Rendering/Core/Camera2D.h"
#include "Rendering/Core/Renderer.h"

#include "editor/systems/GridSystem.h"
#include "editor/utilities/EditorFramebuffers.h"
#include "editor/utilities/EditorUtilities.h"

#include "editor/scene/SceneManager.h"
#include "editor/scene/SceneObject.h"

#include "editor/tools/ToolManager.h"
#include "editor/tools/CreateTileTool.h"

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

	auto& renderSystem = mainRegistry.GetContext<std::shared_ptr<RenderSystem>>();
	auto& renderUISystem = mainRegistry.GetContext<std::shared_ptr<RenderUISystem>>();
	auto& renderShapeSystem = mainRegistry.GetContext<std::shared_ptr<RenderShapeSystem>>();

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

	renderSystem->Update( pCurrentScene->GetRegistry(), *m_pTilemapCam, pCurrentScene->GetLayerParams() );
	renderShapeSystem->Update( pCurrentScene->GetRegistry(), *m_pTilemapCam );
	renderUISystem->Update( pCurrentScene->GetRegistry() );

	auto pActiveTool = SCENE_MANAGER().GetToolManager().GetActiveTool();
	if ( pActiveTool )
		pActiveTool->Draw();

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

	auto pActiveTool = SCENE_MANAGER().GetToolManager().GetActiveTool();
	if ( pActiveTool )
	{
		if ( !pActiveTool->SetupTool( pCurrentScene->GetRegistryPtr(), m_pTilemapCam.get() ) )
		{
			SCION_ASSERT( false && "This should work!!" );
#ifdef _WIN32
			__debugbreak();
#elif __linux
			raise( SIGTRAP );
#endif
		}

		if ( !SCENE_MANAGER().GetCurrentTileset().empty() )
			pActiveTool->LoadSpriteTextureData( SCENE_MANAGER().GetCurrentTileset() );
	}
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

		auto pActiveTool = SCENE_MANAGER().GetToolManager().GetActiveTool();
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

	auto pActiveTool = SCENE_MANAGER().GetToolManager().GetActiveTool();
	if ( pActiveTool && pActiveTool->IsOverTilemapWindow() && !ImGui::GetDragDropPayload() )
	{
		PanZoomCamera( pActiveTool->GetMouseScreenCoords() );

		pActiveTool->Update( pCurrentScene->GetCanvas() );
		pActiveTool->Create();
	}

	m_pTilemapCam->Update();
}

} // namespace SCION_EDITOR
