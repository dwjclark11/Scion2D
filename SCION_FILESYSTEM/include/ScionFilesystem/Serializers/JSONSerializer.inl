#include "JSONSerializer.h"

namespace SCION_FILESYSTEM
{
template <typename TValue>
inline JSONSerializer& JSONSerializer::AddKeyValuePair( const std::string& key, const TValue& value )
{
	m_pWriter->Key( key.c_str() );

	if constexpr ( std::is_same_v<TValue, std::string> )
	{
		m_pWriter->String( value.c_str() );
	}
	else if constexpr ( std::is_integral_v<TValue> )
	{
		m_pWriter->Int64( value );
	}
	else if constexpr ( std::is_unsigned_v<TValue> )
	{
		m_pWriter->Uint64( value );
	}
	else if constexpr ( std::is_floating_point_v<TValue> )
	{
		m_pWriter->Double( value );
	}
	else
	{
		assert( false && "Type not supported!" );
	}

	return *this;
}

} // namespace SCION_FILESYSTEM
