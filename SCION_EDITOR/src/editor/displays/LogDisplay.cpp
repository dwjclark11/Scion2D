#include "editor/displays/LogDisplay.h"
#include "Logger/Logger.h"
#include <ranges>

namespace SCION_EDITOR
{
void LogDisplay::GetLogs()
{
	if ( SCION_LOG_ADDED() )
	{
		Clear();
		std::ranges::reverse_view rLogs{ SCION_GET_LOGS() };

		for ( const auto& log : rLogs )
		{
			int oldTextSize = m_TextBuffer.size();
			m_TextBuffer.append( log.log.c_str() );
			m_TextOffsets.push_back( oldTextSize + 1 );
		}

		SCION_RESET_ADDED();
	}
}

LogDisplay::LogDisplay()
	: m_bAutoScroll{ true }
{
}

void LogDisplay::Clear()
{
	m_TextBuffer.clear();
	m_TextOffsets.clear();
}

void LogDisplay::Draw()
{
	if ( !ImGui::Begin( "Logs" ) )
	{
		ImGui::End();
		return;
	}

	GetLogs();

	ImGui::SameLine();
	if ( ImGui::Button( "Clear" ) )
	{
		Clear();
		SCION_CLEAR_LOGS();
	}
	ImGui::SameLine();

	if ( ImGui::Button( "Copy" ) )
	{
		ImGui::LogToClipboard();
	}
	ImGui::SameLine();
	ImGui::Separator();
	ImGui::BeginChild( "scrolling", ImVec2{ 0.f, 0.f }, false, ImGuiWindowFlags_HorizontalScrollbar );

	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2{ 0.f, 0.f } );
	ImGuiListClipper clipper;

	clipper.Begin( m_TextOffsets.Size );

	while ( clipper.Step() )
	{
		for ( int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++ )
		{
			const char* line_start = m_TextBuffer.begin() + m_TextOffsets[ line_no ] - 1;
			const char* line_end = ( line_no + 1 < m_TextOffsets.Size )
									   ? ( m_TextBuffer.begin() + m_TextOffsets[ line_no + 1 ] - 1 )
									   : m_TextBuffer.end();

			std::string text{ line_start, line_end };
			ImVec4 color{ 1.f, 1.f, 1.f, 1.f };
			if ( text.find( "INFO" ) != std::string::npos )
				color = ImVec4{ 0.f, 1.f, 0.f, 1.f };
			else if ( text.find( "ERROR" ) != std::string::npos )
				color = ImVec4{ 1.f, 0.f, 0.f, 1.f };
			else if ( text.find( "WARN" ) != std::string::npos )
				color = ImVec4{ 1.f, 1.f, 0.f, 1.f };

			ImGui::PushStyleColor( ImGuiCol_Text, color );
			ImGui::TextUnformatted( line_start, line_end );
			ImGui::PopStyleColor();
		}
	}

	clipper.End();
	ImGui::PopStyleVar();

	if ( m_bAutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
		ImGui::SetScrollHereY( 1.f );

	ImGui::EndChild();
	ImGui::End();
}
} // namespace SCION_EDITOR
