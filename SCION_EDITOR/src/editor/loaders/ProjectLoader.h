#pragma once
#include <string>

namespace SCION_CORE
{
class ProjectInfo;
}

namespace SCION_EDITOR
{

class ProjectLoader
{
  public:
	bool CreateNewProject( const std::string& sProjectName, const std::string& sFilepath );
	bool LoadProject( const std::string& sFilepath );
	bool SaveLoadedProject( const SCION_CORE::ProjectInfo& projectInfo );

  private:
	bool CreateProjectFile( const std::string& sProjectName, const std::string& sFilepath );
	bool CreateMainLuaScript( const std::string& sProjectName, const std::string& sFilepath );
	bool CreateScriptListFile( );

};

} // namespace SCION_EDITOR
