#pragma once
#include <fstream>
#include <sstream>
#include <concepts>

namespace Scion::Filesystem
{
template <class T>
concept Streamable = requires( std::ostream& os, T obj ) { os << obj; };

template <Streamable T>
std::string to_string( T val )
{
	if constexpr ( std::is_same_v<T, std::string> )
	{
		return val;
	}
	else
	{
		std::stringstream ss;
		ss << val;
		return ss.str();
	}
}

class LuaSerializer
{
  public:
	explicit LuaSerializer( const std::string& sFilepath );
	~LuaSerializer();

	bool ResetStream( const std::string& sNewFilename );
	bool FinishStream();

	inline const std::string& GetFilepath() const { return m_sFilepath; }

	LuaSerializer& AddComment( const std::string& sComment );
	LuaSerializer& AddBlockComment( const std::string& sComment );

	LuaSerializer& StartNewTable( const std::string& sTableName = "", bool bNewLine = true, bool bBracketed = false,
								  bool bQuoted = false );

	LuaSerializer& EndTable( bool bNewLine = true );

	template <Streamable TValue>
	LuaSerializer& AddValue( const TValue& value, bool bNewLine = true, bool bFinalValue = false,
							 bool bIndent = false, bool bQuote = false );

	template <Streamable TKey, Streamable TValue>
	LuaSerializer& AddKeyValuePair( TKey key, const TValue& value, bool bAddNewLine = true, bool bFinalValue = false,
									bool bQuoteKey = false, bool bQuoteValue = false );

	template<Streamable T>
	LuaSerializer& AddWords( T words, bool bNewLine = false, bool bIndent = false, int indentSize = 1);

  private:
	void AddIndents();
	void AddNewLine();
	void SeparateValues( bool bNewLine = true );

	std::string AddQuotes( const std::string& str );

	template <Streamable T>
	void Stream( const T& val );

  private:
	std::fstream m_FileStream;
	std::string m_sFilepath;
	int m_NumIndents;
	int m_NumTablesStarted;
	bool m_bStreamStarted;
	bool m_bValueAdded;
	bool m_bNewLineAdded;
};


} // namespace Scion::Filesystem

#include "LuaSerializer.inl"
