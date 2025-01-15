#include "ScionUtilities/ScionUtilities.h"

namespace SCION_UTIL
{
std::string_view GetSubstring( const std::string_view& str, const std::string& find )
{
	if ( find.empty() )
		return std::string_view{};

	if ( find.size() > 1 )
	{
		auto const found = str.find( find );
		if ( found == std::string_view::npos )
			return std::string_view{};

		return str.substr( found );
	}

	auto const found = str.find_last_of( find[ 0 ] );
	if ( found == std::string_view::npos )
		return std::string_view{};

	return str.substr( found + 1 );
}
} // namespace SCION_UTIL
