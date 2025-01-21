#include "LuaSerializer.h"

namespace SCION_FILESYSTEM
{
template <Streamable TValue>
inline LuaSerializer& LuaSerializer::AddValue( const TValue& value, bool bNewLine, bool bFinalValue, bool bIndent )
{
	SeparateValues( bNewLine );
	Stream( value );

	if ( bIndent )
		++m_NumIndents;

	m_bNewLineAdded = false;
	m_bValueAdded = !bFinalValue;
	return *this;
}

template <Streamable TKey, Streamable TValue>
inline LuaSerializer& LuaSerializer::AddKeyValuePair( TKey key, const TValue& value, bool bAddNewLine, bool bFinalValue,
													  bool bQuoteKey, bool bQuoteValue )
{
	SeparateValues( bAddNewLine );

	if ( bQuoteKey )
	{
		Stream( "[" + AddQuotes( to_string( key ) ) );
		Stream( "] = " );
	}
	else
	{
		Stream( key );
		Stream( " = " );
	}

	if ( bQuoteValue )
	{
		Stream( AddQuotes( to_string( value ) ) );
	}
	else
	{
		Stream( value );
	}

	m_bNewLineAdded = false;
	m_bValueAdded = !bFinalValue;

	return *this;
}

template <Streamable T>
inline void LuaSerializer::Stream( const T& val )
{
	m_FileStream << val;
}

} // namespace SCION_FILESYSTEM
