#include "ScionFilesystem/Serializers/LuaSerializer.h"
#include "Logger/Logger.h"
#include <set>
#include <algorithm>

constexpr char SEPARATOR = ',';
constexpr char INDENT = '\t';
constexpr char NEW_LINE = '\n';
constexpr char SPACE = ' ';

#define SPECIAL_CASES                                                                                                  \
	std::set<char>                                                                                                     \
	{                                                                                                                  \
		'"', '\\', '\n', '\t', '\r'                                                                                    \
	}

namespace SCION_FILESYSTEM
{
LuaSerializer::LuaSerializer( const std::string& sFilepath )
	: m_FileStream{}
	, m_sFilepath{ sFilepath }
	, m_NumIndents{ 0 }
	, m_NumTablesStarted{ 0 }
	, m_bStreamStarted{ false }
	, m_bValueAdded{ false }
	, m_bNewLineAdded{ false }
{
	m_FileStream.open( sFilepath, std::ios::out | std::ios::trunc );
	SCION_ASSERT( m_FileStream.is_open() /*&&
				  fmt::format( "LuaSerialization failed. Failed to open file [{}]", sFilename )*/ );

	if ( !m_FileStream.is_open() )
		throw std::runtime_error( fmt::format( "LuaSerialization failed. Failed to open file [{}]", sFilepath ) );
}


LuaSerializer::~LuaSerializer()
{
	if ( m_FileStream.is_open() )
		m_FileStream.close();
}

bool LuaSerializer::ResetStream( const std::string& sNewFilename )
{
	if ( m_FileStream.is_open() )
	{
		m_FileStream.close();
		m_bNewLineAdded = false;
		m_bValueAdded = false;
		m_bStreamStarted = false;
		m_sFilepath = sNewFilename;
	}

	m_FileStream.open( sNewFilename, std::ios::out | std::ios::trunc );
	return m_FileStream.is_open();
}

bool LuaSerializer::FinishStream()
{
	SCION_ASSERT( m_NumTablesStarted == 0 && "Too many tables started! Did you forget to call EndTable()?" );
	SCION_ASSERT( m_NumIndents == 0 && "Indent count should be zero when ending the document!" );
	Stream( NEW_LINE );
	return true;
}

LuaSerializer& LuaSerializer::AddComment( const std::string& sComment )
{
	// Start the comment Lua comment --
	Stream( "-- " + sComment + "\n" );
	return *this;
}

LuaSerializer& LuaSerializer::AddBlockComment( const std::string& sComment )
{
	// Start the comment Lua --[[ block comment ]]
	Stream( "--[[\n" + sComment );
	Stream( '\n' );
	Stream( "--]]\n" );
	return *this;
}

LuaSerializer& LuaSerializer::StartNewTable( const std::string& sTableName, bool bNewLine, bool bBracketed,
											 bool bQuoted )
{
	++m_NumTablesStarted;
	SeparateValues( bNewLine );

	if ( !sTableName.empty() )
	{
		if ( bBracketed )
		{
			Stream( "[" + ( bQuoted ? AddQuotes( sTableName ) : sTableName ) + "]" );
		}
		else
		{
			Stream( sTableName );
		}

		Stream( " = { " );
	}
	else // Table with no key --> should use integers
	{
		Stream( "{ " );
	}

	++m_NumIndents;
	m_bNewLineAdded = false;
	m_bValueAdded = false;

	return *this;
}

LuaSerializer& LuaSerializer::EndTable( bool bNewLine )
{
	--m_NumTablesStarted;
	--m_NumIndents;
	SeparateValues( bNewLine );
	Stream( '}' );
	m_bNewLineAdded = false;
	m_bValueAdded = true;

	return *this;
}

void LuaSerializer::AddIndents()
{
	std::string indent{  };
	for ( int i = 0; i < m_NumIndents; i++ )
	{
		indent += INDENT;
	}

	Stream( indent );
}
void LuaSerializer::AddNewLine()
{
	if ( m_bNewLineAdded )
		return;

	Stream( NEW_LINE );
	AddIndents();
}

void LuaSerializer::SeparateValues( bool bNewLine )
{
	if ( !m_bValueAdded && bNewLine )
	{
		AddNewLine();
		return;
	}

	if ( m_bValueAdded )
	{
		Stream( SEPARATOR );
		Stream( SPACE );
	}

	m_bValueAdded = false;
	if ( bNewLine )
		AddNewLine();
}

std::string LuaSerializer::AddQuotes( const std::string& str )
{
	std::string quotedStr{ "\"" };
	std::string stringToQuote{ str };

	auto it = stringToQuote.begin();
	while ( it != stringToQuote.end() )
	{
		it = std::find_if( it, stringToQuote.end(), [ & ]( char c ) { return SPECIAL_CASES.contains( c ); } );

		if ( it != stringToQuote.end() )
			it = stringToQuote.insert( it + 1, '\\' ) + 1;
	}

	quotedStr += stringToQuote + "\"";
	return quotedStr;
}
} // namespace SCION_FILESYSTEM
