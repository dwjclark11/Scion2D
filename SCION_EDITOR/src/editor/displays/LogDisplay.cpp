#include "editor/displays/LogDisplay.h"
#include "editor/utilities/fonts/IconsFontAwesome5.h"
#include "Logger/Logger.h"
#include <imgui_stdlib.h>

namespace Scion::Editor
{
void LogDisplay::GetLogs()
{
	const auto& logs{ SCION_GET_LOGS() };
	if ( logs.size() == m_LogIndex )
		return;

	for ( size_t i = m_LogIndex; i < logs.size(); ++i )
	{
		const auto& log = logs[ i ];
		int oldTextSize = m_TextBuffer.size();
		m_TextBuffer.append( log.log.c_str() );
		m_TextBuffer.append( "\n" );
		m_TextOffsets.push_back( oldTextSize );
	}

	m_LogIndex = static_cast<int>( logs.size() );
}

LogDisplay::LogDisplay()
	: m_sSearchQuery{  }
	, m_bAutoScroll{ true }
	, m_bShowInfo{ true }
	, m_bShowWarn{ true }
	, m_bShowError{ true }
	, m_LogIndex{ 0 }
{
}

void LogDisplay::Clear()
{
	m_TextBuffer.clear();
	m_TextOffsets.clear();
	m_LogIndex = 0;
}

void LogDisplay::Draw()
{
	if ( !ImGui::Begin( ICON_FA_TERMINAL " Logs" ) )
	{
		ImGui::End();
		return;
	}

	GetLogs();

	// Logging Controls
	ImGui::Text( "Search:" );
	ImGui::SameLine();
	ImGui::SetNextItemWidth( 200.f );
	ImGui::InputText( "##Search", &m_sSearchQuery );
	ImGui::SameLine( 0.f, 8.f );
	ImGui::Checkbox( " INFO", &m_bShowInfo );
	ImGui::SameLine( 0.f, 8.f );
	ImGui::Checkbox( " WARN", &m_bShowWarn );
	ImGui::SameLine( 0.f, 8.f );
	ImGui::Checkbox( " ERROR", &m_bShowError );
	ImGui::SameLine( 0.f, 8.f );
	ImGui::Checkbox( " Auto-scroll", &m_bAutoScroll );
	ImGui::SameLine( 0.f, 16.f );
	if ( ImGui::Button( "Clear" ) )
	{
		Clear();
		SCION_CLEAR_LOGS();
	}
	ImGui::SameLine( 0.f, 16.f );

	if ( ImGui::Button( "Copy" ) )
	{
		ImGui::LogToClipboard();
	}

	ImGui::Separator();

	if ( ImGui::BeginChild( "scrolling", ImVec2{ 0.f, 0.f }, false, ImGuiWindowFlags_HorizontalScrollbar ) )
	{
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f } );

		for ( int i = 0; i < m_TextOffsets.Size; ++i )
		{
			const char* line_start = m_TextBuffer.begin() + m_TextOffsets[ i ];
			const char* line_end = ( i + 1 < m_TextOffsets.Size )
									   ? ( m_TextBuffer.begin() + m_TextOffsets[ i + 1 ] - 1 )
									   : m_TextBuffer.end();

			std::string_view text{ line_start, line_end };

			// Apply Filters
			bool bIsInfo{ text.find( "INFO" ) != std::string_view::npos };
			bool bIsWarn{ text.find( "WARN" ) != std::string_view::npos };
			bool bIsError{ text.find( "ERROR" ) != std::string_view::npos };

			if ( ( bIsInfo && !m_bShowInfo ) || ( bIsWarn && !m_bShowWarn ) || ( bIsError && !m_bShowError ) )
				continue;

			if ( !m_sSearchQuery.empty() && text.find( m_sSearchQuery ) == std::string_view::npos )
				continue;

				ImVec4 color{ 1.f, 1.f, 1.f, 1.f };
			if ( bIsInfo )
				color = ImVec4{ 0.f, 1.f, 0.f, 1.f };
			else if ( bIsError )
				color = ImVec4{ 1.f, 0.f, 0.f, 1.f };
			else if ( bIsWarn )
				color = ImVec4{ 1.f, 1.f, 0.f, 1.f };

			ImGui::PushStyleColor( ImGuiCol_Text, color );
			ImGui::TextUnformatted( line_start, line_end );
			ImGui::PopStyleColor();
		}

		ImGui::PopStyleVar();

		if ( m_bAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
			ImGui::SetScrollHereY( 1.f );

		ImGui::EndChild();
	}
	ImGui::End();
}
} // namespace Scion::Editor
