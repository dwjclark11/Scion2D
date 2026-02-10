#pragma once
#include <string>
#include <vector>

namespace Scion::Filesystem
{
class FileProcessor
{
  public:
	bool OpenApplicationFromFile( const std::string& sFilename, std::vector<const char*> params );
	bool OpenFileLocation( const std::string& sFilename );

  private:
	std::string SeparateParams( std::vector<const char*>& params )
	{
		std::string sParams{};
		for ( const auto& param : params )
			sParams += param + ' ';

		return sParams;
	}

};
} // namespace Scion::Filesystem
