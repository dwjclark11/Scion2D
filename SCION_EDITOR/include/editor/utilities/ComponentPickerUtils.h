#pragma once
#include <entt/entt.hpp>

namespace Scion::Editor
{

struct ComponentPickerEntry
{
	entt::id_type typeId;
	std::string sDisplayName;
	std::string sCategory;
	int32_t nOrder;
};

// Builds a sorted, filtered list of every meta type tagged META_PROP_IS_COMPONENT.
// Call once per frame or cache and rebuild when needed.
std::vector<ComponentPickerEntry> BuildComponentList();

// Draws a BeginCombo dropdown with category separators and a search bar.
// Returns the typeId of the chosen component, or entt::id_type{0} if nothing
// was chosen this frame.
// Pass in a persistent sSearchBuf (std::string) owned by the caller.
entt::id_type DrawComponentPicker( const char* sComboLabel, const std::vector<ComponentPickerEntry>& entries,
								   std::string& sSearchBuf, const std::string& sCurrentName = {} );

} // namespace Scion::Editor
