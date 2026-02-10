#include "LuaSerializer.h"

namespace Scion::Filesystem
{
template <Streamable TValue>
inline LuaSerializer& LuaSerializer::AddValue( const TValue& value, bool bNewLine, bool bFinalValue, bool bIndent,
											   bool bQuote )
{
	SeparateValues( bNewLine );
	Stream( bQuote ? AddQuotes(to_string(value)) : value );

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
inline LuaSerializer& LuaSerializer::AddWords( T words, bool bNewLine, bool bIndent, int indentSize )
{
	if ( bNewLine )
		AddNewLine();

	if ( bIndent )
	{
		m_NumIndents += indentSize;
		AddIndents();
		m_NumIndents -= indentSize;
	}

	Stream( words );
	m_bNewLineAdded = false;
	m_bValueAdded = false;

	return *this;
}

template <Streamable T>
inline void LuaSerializer::Stream( const T& val )
{
	if constexpr ( std::is_same_v<T, bool> )
	{
		auto flags = m_FileStream.flags(); // save stream state
		m_FileStream << std::boolalpha << val;
		m_FileStream.flags( flags ); // restore
	}
	else
	{
		m_FileStream << val;
	}
}

} // namespace Scion::Filesystem
