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

namespace Scion::Utilities
{

enum class AssetType
{
	TEXTURE = 0,
	FONT,
	SOUNDFX,
	MUSIC,
	SCENE,
	SHADER,
	PREFAB,

	NO_TYPE
};

/*
 * struct S2DAsset
 * Helper struct used for loading zipped or archived assets.
 * Archived assets are the games assets that have been converted into luac files.
 */
struct S2DAsset
{
	/* The name of the asset. */
	std::string sName{};
	/* The size of the asset data. Based on a char array. */
	size_t assetSize{ 0 };
	/* The end position of the asset data. */
	size_t assetEnd{ 0 };
	/* The type of asset that the data represents. */
	AssetType eType{ AssetType::NO_TYPE };
	/* The underlying hex data of the asset. */
	std::vector<unsigned char> assetData;
	/* Optional parameter if asset is font. */
	std::optional<float> optFontSize{ std::nullopt };
	/* Optional parameter if asset is a texture. */
	std::optional<bool> optPixelArt{ std::nullopt };
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

std::string AssetTypeToStr( AssetType eAssetType );
AssetType StrToAssetType( const std::string& sAssetType );

namespace StringUtils
{
std::string GetSubstring( std::string_view str, std::string_view find );

std::string ConvertWideToANSI( const std::wstring& wstr );
std::wstring ConvertAnsiToWide( const std::string& str );
std::string ConvertWideToUtf8( const std::wstring& wstr );
std::wstring ConvertUtf8ToWide( const std::string& str );

template <typename CharT, typename Traits, typename Alloc>
inline void RemoveSuffixIfPresent( std::basic_string<CharT, Traits, Alloc>& str,
								   std::basic_string_view<CharT, Traits> suffix )
{
	if ( str.ends_with( suffix ) )
	{
		str.erase( str.size() - suffix.size() );
	}
}

template <typename CharT, typename Traits, typename Alloc>
inline void RemovePrefixIfPresent( std::basic_string<CharT, Traits, Alloc>& str,
								   std::basic_string_view<CharT, Traits> prefix )
{
	if ( str.starts_with( prefix ) )
	{
		str.erase( 0, prefix.size() );
	}
}

// Return a copy with suffix removed (if present)
template <typename CharT, typename Traits, typename Alloc>
[[nodiscard]] inline std::basic_string<CharT, Traits, Alloc> RemoveSuffixCopy(
	std::basic_string<CharT, Traits, Alloc> str, std::basic_string_view<CharT, Traits> suffix )
{
	RemoveSuffixIfPresent( str, suffix );
	return str;
}

// Overload for suffix (string literal)
template <typename CharT, typename Traits, typename Alloc, size_t N>
[[nodiscard]] inline std::basic_string<CharT, Traits, Alloc> RemoveSuffixCopy(
	std::basic_string<CharT, Traits, Alloc> str, const CharT ( &suffix )[ N ] )
{
	RemoveSuffixIfPresent( str, std::basic_string_view<CharT>( suffix, N - 1 ) );
	return str;
}

// Return a copy with prefix removed (if present)
template <typename CharT, typename Traits, typename Alloc>
[[nodiscard]] inline std::basic_string<CharT, Traits, Alloc> RemovePrefixCopy(
	std::basic_string<CharT, Traits, Alloc> str, std::basic_string_view<CharT, Traits> prefix )
{
	RemovePrefixIfPresent( str, prefix );
	return str;
}

// Overload for prefix (string literal)
template <typename CharT, typename Traits, typename Alloc, size_t N>
[[nodiscard]] inline std::basic_string<CharT, Traits, Alloc> RemovePrefixCopy(
	std::basic_string<CharT, Traits, Alloc> str, const CharT ( &prefix )[ N ] )
{
	RemovePrefixIfPresent( str, std::basic_string_view<CharT>( prefix, N - 1 ) );
	return str;
}
} // namespace StringUtils
} // namespace Scion::Utilities

#define GET_SUBSTRING( str, find ) Scion::Utilities::StringUtils::GetSubstring( str, find )
