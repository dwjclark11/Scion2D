#include "ScionUtilities/ScionUtilities.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

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

} // namespace SCION_UTIL
