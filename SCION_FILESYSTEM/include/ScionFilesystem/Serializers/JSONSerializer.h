#pragma once
#include <fstream>
#include <rapidjson/prettywriter.h>

namespace SCION_FILESYSTEM
{
class JSONSerializer
{
  private:
	std::fstream m_Filestream;
	rapidjson::StringBuffer m_StringBuffer;
	std::unique_ptr<rapidjson::PrettyWriter<rapidjson::StringBuffer>> m_pWriter;
	int m_NumObjectsStarted;
	int m_NumArraysStarted;

  public:
	JSONSerializer( const std::string& sFilename, int maxDecimalPlaces = -1 );
	~JSONSerializer();

	bool StartDocument();
	bool EndDocument();

	bool Reset( const std::string& sFilename );

	JSONSerializer& StartNewObject( const std::string& key = "" );
	JSONSerializer& EndObject();
	JSONSerializer& StartNewArray( const std::string& key );
	JSONSerializer& EndArray();

	JSONSerializer& AddKeyValuePair( const std::string& key, const bool& value );

	template <typename TValue>
	JSONSerializer& AddKeyValuePair( const std::string& key, const TValue& value );
};

} // namespace SCION_FILESYSTEM

#include "JSONSerializer.inl"
