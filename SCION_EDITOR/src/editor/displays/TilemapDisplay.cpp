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

#include "Logger/Logger.h"
#include <imgui.h>

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

	if (!pCurrentScene)
	{
		fb->Unbind();
		return;
	}

	auto& gridSystem = mainRegistry.GetContext<std::shared_ptr<GridSystem>>();
	gridSystem->Update( *pCurrentScene, *m_pTilemapCam );

	renderSystem->Update( pCurrentScene->GetRegistry());
	renderShapeSystem->Update( pCurrentScene->GetRegistry() );
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
	if (pActiveTool)
	{
		if (!pActiveTool->SetupTool(pCurrentScene->GetRegistryPtr(), m_pTilemapCam.get()))
		{
			SCION_ASSERT( false && "This should work!!" );
			__debugbreak();
		}

		if ( !SCENE_MANAGER().GetCurrentTileset().empty() )
			pActiveTool->LoadSpriteTextureData( SCENE_MANAGER().GetCurrentTileset() );
	}
}

TilemapDisplay::TilemapDisplay()
	: m_pTilemapCam{ std::make_unique<SCION_RENDERING::Camera2D>() }
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
		if (pActiveTool)
		{
			pActiveTool->SetRelativeCoords( glm::vec2{ relativePos.x, relativePos.y } );
			pActiveTool->SetCursorCoords( glm::vec2{ io.MousePos.x, io.MousePos.y } );
			pActiveTool->SetWindowPos( glm::vec2{ windowPos.x, windowPos.y } );
			pActiveTool->SetWindowSize( glm::vec2{ windowSize.x, windowSize.y } );
		}

		ImGui::Image( (ImTextureID)fb->GetTextureID(), imageSize, ImVec2{ 0.f, 1.f }, ImVec2{ 1.f, 0.f } );

		// Accept Scene Drop Target
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( DROP_SCENE_SRC );
			if (payload)
			{
				SCENE_MANAGER().SetCurrentScene( std::string{ (const char*)payload->Data } );
				LoadNewScene();				
			}

			ImGui::EndDragDropTarget();
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
	if (pActiveTool && !ImGui::GetDragDropPayload())
	{
		pActiveTool->Update( pCurrentScene->GetCanvas() );
		pActiveTool->Create();
	}

	m_pTilemapCam->Update();
}

} // namespace SCION_EDITOR