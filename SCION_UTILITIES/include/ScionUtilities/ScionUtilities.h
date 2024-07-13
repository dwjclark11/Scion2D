#pragma once
#include <vector>
#include <algorithm>
#include <ranges>

namespace SCION_UTIL
{
template <typename Map>
std::vector<typename Map::key_type> GetKeys( const Map& map )
{
	auto keyView = std::views::keys( map );
	std::vector<typename Map::key_type> keys{ keyView.begin(), keyView.end() };
	return keys;
}

template <typename Map, typename Func>
std::vector<typename Map::key_type> GetKeys( const Map& map, Func func )
{
	auto keyView = map | std::views::filter( func ) | std::views::keys;
	std::vector<typename Map::key_type> keys{ keyView.begin(), keyView.end() };
	return keys;
}
} // namespace SCION_UTIL