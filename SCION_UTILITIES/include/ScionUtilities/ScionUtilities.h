#pragma once
#include <vector>
#include <algorithm>
#include <ranges>
#include <functional>
#include <concepts>
#include <string_view>
#include <set>
#include <map>
#include <unordered_map>
#include <string>

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

/* Ensure the types that are passed in are associative map types. */
template <typename T>
concept MapType = std::same_as<T, std::map<typename T::key_type, typename T::mapped_type, typename T::key_compare,
										   typename T::allocator_type>> ||
				  std::same_as<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher,
													 typename T::key_equal, typename T::allocator_type>>;

/*
 * @brief Returns a vector of the keys of a specified map.
 * @tparam The map type that is passed in.
 * @param A const map& to extract the keys.
 * @return Returns a vector of the key_type of the map.
 */
template <MapType Map>
std::vector<typename Map::key_type> GetKeys( const Map& map )
{
	auto keyView = std::views::keys( map );
	std::vector<typename Map::key_type> keys{ keyView.begin(), keyView.end() };
	return keys;
}

/*
 * @brief Returns a vector of filtered keys of a specified map.
 * @tparam The map type that is passed in.
 * @tparam The predicate function to filter the keys.
 * @param A const map& to extract the keys.
 * @return Returns a vector of the key_type of the map.
 */
template <MapType Map, typename Func>
std::vector<typename Map::key_type> GetKeys( const Map& map, Func func )
{
	auto keyView = map | std::views::filter( func ) | std::views::keys;
	std::vector<typename Map::key_type> keys{ keyView.begin(), keyView.end() };
	return keys;
}

/*
 * @brief Change the specified key of a map.
 * @tparam The map type that is passed in.
 * @param A reference to the map that we want to change the key.
 * @param The key to be changed, must be of the same type as the maps keys.
 * @param value to change the key to, must be of the same type as the maps keys.
 * @return Returns true if successful, false otherwise.
 */
template <MapType Map>
bool KeyChange( Map& map, const typename Map::key_type& key, const typename Map::key_type& change )
{
	if ( !map.contains( key ) || map.contains( change ) )
	{
		return false;
	}

	auto node = map.extract( key );
	node.key() = change;
	const auto [ itr, bSuccess, nType ] = map.insert( std::move( node ) );
	return bSuccess;
}

template <typename TCont, typename TFunc>
bool CheckContainsValue( TCont& cont, TFunc func )
{
	auto itr = std::ranges::find_if( cont, func );
	return itr == cont.end();
}

template <typename T>
concept UintType = requires( T param ) { requires std::is_integral_v<T> && std::is_unsigned_v<T>; };

template <UintType T>
T SetBit( T num, int pos )
{
	assert( pos < std::numeric_limits<T>::digits && "pos must be less than the number of digits." );
	T mask = 1 << pos;
	return num | mask;
}

/*
 *@brief Returns the first set bit in the given number.
 */
template <UintType T>
T GetFirstSetBit( T num )
{
	int position{ 1 }, i{ 1 };
	while ( !( i & num ) )
	{
		i <<= 1;
		++position;
	}

	return position;
}

template <UintType T>
T Bit( const T& x )
{
	return T( 1 ) << x;
}

template <UintType T>
std::set<T> GetAllSetBits( T num )
{
	std::set<T> setBits{};
	int position{ 0 };
	while ( num )
	{
		if ( num & 1 )
			setBits.insert( position );

		++position;
		num >>= 1;
	}

	return setBits;
}

template <UintType T>
void ResetBit( T& num, int bit )
{
	num &= ~( 1 << bit );
}

template <UintType T>
bool IsBitSet( const T& x, const T& y )
{
	return 0 != ( x & y );
}

std::string_view GetSubstring( const std::string_view& str, const std::string& find );

std::string ConvertWideToANSI( const std::wstring& wstr );
std::wstring ConvertAnsiToWide( const std::string& str );
std::string ConvertWideToUtf8( const std::wstring& wstr );
std::wstring ConvertUtf8ToWide( const std::string& str );

} // namespace SCION_UTIL

#define GET_SUBSTRING( str, find ) SCION_UTIL::GetSubstring( str, find )
