#include "editor/utilities/ComponentPickerUtils.h"
#include "Core/ECS/Components/ComponentMetaTraits.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

using namespace Scion::Core::ECS;

namespace Scion::Editor
{

std::vector<ComponentPickerEntry> BuildComponentList()
{
	std::vector<ComponentPickerEntry> entries;

	for ( auto&& [ id, metaType ] : entt::resolve() )
	{
		// Filter: only types tagged IsComponent via .traits()
		const auto traitVal = metaType.traits<EMetaTraits>();
		const bool bIsComp = ( traitVal & EMetaTraits::IsComponent ) == EMetaTraits::IsComponent;
		if ( !bIsComp )
			continue;

		ComponentPickerEntry entry{};
		entry.typeId = id;

		// Read category and order from .custom()
		const ComponentMetaInfo* pInfo = metaType.custom();

		if ( pInfo )
		{
			entry.sCategory = pInfo->sCategory;
			entry.nOrder = pInfo->nOrder;
		}
		else
		{
			entry.sCategory = "General";
			entry.nOrder = 999;
		}

		std::string fullName{ metaType.info().name() };
		auto lastColon = fullName.rfind( ':' );
		entry.sDisplayName = ( lastColon != std::string::npos ) ? fullName.substr( lastColon + 1 ) : fullName;

		entries.push_back( std::move( entry ) );
	}

	std::ranges::sort( entries, []( const ComponentPickerEntry& a, const ComponentPickerEntry& b ) {
		if ( a.sCategory != b.sCategory )
			return a.sCategory < b.sCategory;
		if ( a.nOrder != b.nOrder )
			return a.nOrder < b.nOrder;
		return a.sDisplayName < b.sDisplayName;
	} );

	return entries;
}

entt::id_type DrawComponentPicker( const char* sComboLabel, const std::vector<ComponentPickerEntry>& entries,
								   std::string& sSearchBuf, const std::string& sCurrentName )
{
	entt::id_type chosenId{ 0 };

	if ( !ImGui::BeginCombo( sComboLabel, sCurrentName.c_str() ) )
		return chosenId;

	// Search bar pinned at the top of the popup
	ImGui::SetNextItemWidth( -1.f );
	ImGui::InputTextWithHint( "##comp_search", "Search...", &sSearchBuf );

	ImGui::Separator();

	std::string sLastCategory{};

	for ( const auto& entry : entries )
	{
		// Apply search filter
		if ( !sSearchBuf.empty() )
		{
			// Case-insensitive substring check
			auto it = std::search(
				entry.sDisplayName.begin(),
				entry.sDisplayName.end(),
				sSearchBuf.begin(),
				sSearchBuf.end(),
				[]( char a, char b ) { return std::tolower( (unsigned char)a ) == std::tolower( (unsigned char)b ); } );
			if ( it == entry.sDisplayName.end() )
				continue;
		}

		// Draw a separator + category header whenever the category changes.
		// Skip category headers when a search is active so the list stays clean.
		if ( sSearchBuf.empty() && entry.sCategory != sLastCategory )
		{
			if ( !sLastCategory.empty() )
				ImGui::Separator();

			ImGui::TextDisabled( "%s", entry.sCategory.c_str() );
			sLastCategory = entry.sCategory;
		}

		ImGui::PushID( static_cast<int>( entry.typeId ) );

		bool bSelected = ( entry.sDisplayName == sCurrentName );
		if ( ImGui::Selectable( entry.sDisplayName.c_str(), bSelected ) )
		{
			chosenId = entry.typeId;
			sSearchBuf.clear();
			ImGui::CloseCurrentPopup();
		}

		if ( bSelected )
			ImGui::SetItemDefaultFocus();

		ImGui::PopID();
	}

	ImGui::EndCombo();
	return chosenId;
}

} // namespace Scion::Editor
