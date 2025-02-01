#include "ScionFilesystem/Serializers/JSONSerializer.h"
#include "Logger/Logger.h"

constexpr int MAX_DECIMAL_PLACES = 5;

namespace SCION_FILESYSTEM
{
JSONSerializer::JSONSerializer( const std::string& sFilename, int maxDecimalPlaces )
	: m_Filestream{}
	, m_StringBuffer{}
	, m_pWriter{ std::make_unique<rapidjson::PrettyWriter<rapidjson::StringBuffer>>( m_StringBuffer ) }
	, m_NumObjectsStarted{ 0 }
	, m_NumArraysStarted{ 0 }
{
	m_Filestream.open( sFilename, std::ios::out | std::ios::trunc );
	SCION_ASSERT( m_Filestream.is_open() && "Failed to open file!" );

	if ( !m_Filestream.is_open() )
		throw std::runtime_error( fmt::format( "JSONSerializer failed to open file [{}]", sFilename ) );

	m_pWriter->SetMaxDecimalPlaces( maxDecimalPlaces > 1 ? maxDecimalPlaces : MAX_DECIMAL_PLACES );
}

JSONSerializer::~JSONSerializer()
{
	if ( m_Filestream.is_open() )
		m_Filestream.close();
}

bool JSONSerializer::StartDocument()
{
	SCION_ASSERT( m_NumObjectsStarted == 0 && "Document has already been started. Please Reset the serializer." );

	if ( m_NumObjectsStarted != 0 )
	{
		SCION_ERROR( "Document has already been started. Please Reset the serializer." );
		return false;
	}

	++m_NumObjectsStarted;

	return m_pWriter->StartObject();
}

bool JSONSerializer::EndDocument()
{
	SCION_ASSERT( m_NumObjectsStarted == 1 && "There should only be one object open." );
	SCION_ASSERT( m_NumArraysStarted == 0 && "Too many arrays started. Did you forget to call EndArray()?" );

	if ( m_NumObjectsStarted != 1 )
	{
		SCION_ERROR( "Failed to end document correctly. There should only be one object open. Did you forget to call "
					 "EndObject()?" );
		return false;
	}

	if ( m_NumArraysStarted != 0 )
	{
		SCION_ERROR( "Failed to end document correctly. Too many arrays started. Did you forget to call "
					 "EndArray()?" );
		return false;
	}

	m_pWriter->EndObject();
	--m_NumObjectsStarted;

	m_Filestream << m_StringBuffer.GetString();
	m_Filestream.flush();

	return true;
}

bool JSONSerializer::Reset( const std::string& sFilename )
{
	SCION_ASSERT( m_NumObjectsStarted == 0 && "Cannot Reset, Document has not been finished." );
	SCION_ASSERT( m_NumArraysStarted == 0 && "Cannot reset, Too many arrays open" );

	if ( m_NumObjectsStarted != 0 )
	{
		SCION_ERROR( "Cannot Reset, Document has not been finished." );
		return false;
	}

	if ( m_NumArraysStarted != 0 )
	{
		SCION_ERROR( "Cannot reset, Too many arrays open" );
		return false;
	}

	if ( m_Filestream.is_open() )
		m_Filestream.close();

	m_Filestream.open( sFilename, std::ios::out | std::ios::trunc );
	SCION_ASSERT( m_Filestream.is_open() && "Failed to open file!" );

	if ( !m_Filestream.is_open() )
		throw std::runtime_error( fmt::format( "JSONSerializer failed to open file [{}]", sFilename ) );

	return true;
}

JSONSerializer& JSONSerializer::StartNewObject( const std::string& key )
{
	++m_NumObjectsStarted;
	if ( !key.empty() )
		m_pWriter->Key( key.c_str() );

	m_pWriter->StartObject();
	return *this;
}
JSONSerializer& JSONSerializer::EndObject()
{
	SCION_ASSERT( m_NumObjectsStarted > 1 && "EndObject() called too many times!" );
	--m_NumObjectsStarted;
	m_pWriter->EndObject();
	return *this;
}

JSONSerializer& JSONSerializer::StartNewArray( const std::string& key )
{
	++m_NumArraysStarted;
	m_pWriter->Key( key.c_str() );

	m_pWriter->StartArray();
	return *this;
}

JSONSerializer& JSONSerializer::EndArray()
{
	SCION_ASSERT( m_NumArraysStarted > 0 && "EndArray() called too many times!" );
	--m_NumArraysStarted;
	m_pWriter->EndArray();
	return *this;
}

JSONSerializer& JSONSerializer::AddKeyValuePair( const std::string& key, const bool& value )
{
	m_pWriter->Key( key.c_str() );
	m_pWriter->Bool( value );

	return *this;
}

} // namespace SCION_FILESYSTEM
