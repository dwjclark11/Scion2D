#include "MenuDisplay.h"
#include "Logger/Logger.h"
#include "ScionFilesystem/Dialogs/FileDialog.h"

#include <imgui.h>
#include <SDL.h>

namespace SCION_EDITOR
{
void MenuDisplay::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "Ctrl + N"))
			{
				SCION_LOG( "NEW PRESSED" );
			}

			if ( ImGui::MenuItem( "Open", "Ctrl + O" ) )
			{
				SCION_FILESYSTEM::FileDialog fd{};
				auto file = fd.OpenFileDialog( "Open tests", SDL_GetBasePath(), { "*.png", "*.jpg" } );

				if (!file.empty())
				{
					SCION_LOG( "FILE OPENED: {}", file );
				}
			}

			if ( ImGui::MenuItem( "Save", "Ctrl + S" ) )
			{
				SCION_LOG( "SAVE PRESSED" );
			}

			if ( ImGui::MenuItem( "Exit") )
			{
				SCION_LOG( "SHOULD EVENTUALLY EXIT!" );				
			}

			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Edit" ) )
		{
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Tools" ) )
		{
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Settings" ) )
		{
			ImGui::EndMenu();
		}

		if ( ImGui::BeginMenu( "Help" ) )
		{
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
} // namespace SCION_EDITOR
