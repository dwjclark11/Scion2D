#pragma once
#include <vector>
#include <algorithm>
#include <ranges>

namespace SCION_UTIL
{

enum class AssetType
{
	TEXTURE = 0,
	FONT,
	SOUNDFX,
	MUSIC,
	SCENE,

	NO_TYPE
};

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

template <typename Map>
bool KeyChange(Map& map, const typename Map::key_type& key, const typename Map::key_type& change)
{
	if (!map.contains(key) || map.contains(change))
	{
		return false;
	}

	auto node = map.extract( key );
	node.key() = change;
	const auto [itr, bSuccess, nType] = map.insert( std::move( node ) );
	return bSuccess;
}

} // namespace SCION_UTIL