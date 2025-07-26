#include "ScionUtilities/ScionUtilities.h"
#include <random>
#include <array>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace SCION_UTIL
{
std::string GetSubstring( std::string_view str, std::string_view find )
{
	if ( find.empty() )
		return {};

	if ( find.size() > 1 )
	{
		auto const found = str.find( find );
		if ( found == std::string_view::npos )
			return {};

		return std::string{ str.substr( found ) };
	}

	auto const found = str.find_last_of( find[ 0 ] );
	if ( found == std::string_view::npos || found + 1 >= str.size() )
		return {};

	return std::string{ str.substr( found + 1 ) };
}

std::string ConvertWideToANSI( const std::wstring& wstr )
{
#ifdef _WIN32
	int count = WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL );
	std::string str( count, 0 );
	WideCharToMultiByte( CP_ACP, 0, wstr.c_str(), -1, &str[ 0 ], count, NULL, NULL );
	return str;
#else
	return {};
#endif
}

std::wstring ConvertAnsiToWide( const std::string& str )
{

#ifdef _WIN32
	int count = MultiByteToWideChar( CP_ACP, 0, str.c_str(), str.length(), NULL, 0 );
	std::wstring wstr( count, 0 );
	MultiByteToWideChar( CP_ACP, 0, str.c_str(), str.length(), &wstr[ 0 ], count );
	return wstr;
#else
	return {};
#endif
}
std::string ConvertWideToUtf8( const std::wstring& wstr )
{

#ifdef _WIN32
	int count = WideCharToMultiByte( CP_UTF8, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL );
	std::string str( count, 0 );
	WideCharToMultiByte( CP_UTF8, 0, wstr.c_str(), -1, &str[ 0 ], count, NULL, NULL );
	return str;
#else
	return {};
#endif
}
std::wstring ConvertUtf8ToWide( const std::string& str )
{

#ifdef _WIN32
	int count = MultiByteToWideChar( CP_UTF8, 0, str.c_str(), str.length(), NULL, 0 );
	std::wstring wstr( count, 0 );
	MultiByteToWideChar( CP_UTF8, 0, str.c_str(), str.length(), &wstr[ 0 ], count );
	return wstr;
#else
	return {};
#endif
}

std::string GenerateGUID()
{
	// Use a secure random device as the seed source
	std::random_device rd;
	// Seed a Mersenne Twister 32-bit engine with the random device
	std::mt19937 gen( rd() );
	// Create distributions for different segment sizes of the UUID
	std::uniform_int_distribution<uint32_t> dist32; // 8 hex digits
	std::uniform_int_distribution<uint16_t> dist16;	// 4 hex digits
	std::uniform_int_distribution<uint16_t> dist8( 0, 255 ); // 2 hex digits (used for final segment)

	std::stringstream ss;
	ss	<< std::hex << std::setfill( '0' )
		// First Segment: 8 hex digits (32-bits)
		<< std::setw( 8 ) << dist32( gen ) << '-'
		// Second Segment: 4 hex digits (16-bits)
		<< std::setw( 4 ) << dist16( gen ) << '-'
		// Third Segment: 4 hex digits, version 4 UUID
		// Force the first four bits to 0100 (UUID Version 4)
		<< std::setw( 4 ) << ( dist16( gen ) & 0x0FFF | 0x4000 ) << '-'
		// Fourth Segment: 4 hex digits, variant 1(10xx)
		// Force the first 2 bits to 10 for variant 1 UUID
		<< std::setw( 4 ) << ( dist16( gen ) & 0x3FFF | 0x8000 ) << '-';		  

	// Final Segment: 12 hex digits (6 bytes, each digits)
	for ( int i = 0; i < 6; ++i )
	{
		//ss << std::setw( 2 ) << std::hex << static_cast<int>( dist8( gen ) );
		ss << std::setw( 2 ) << std::hex << std::setfill('0') << dist8( gen );
	}

	return ss.str();
}

} // namespace SCION_UTIL
