#pragma once
#include <string>

namespace SCION_CORE
{
struct SaveProject;
}

namespace SCION_EDITOR
{

class SaveProject;

class ProjectLoader
{
  public:
	bool CreateNewProject( const std::string& sProjectName, const std::string& sFilepath );
	bool LoadProject( const std::string& sFilepath );
	bool SaveLoadedProject( SCION_CORE::SaveProject& save );

  private:
	bool CreateProjectFile( const std::string& sProjectName, const std::string& sFilepath );
	bool CreateMainLuaScript( const std::string& sProjectName, const std::string& sFilepath );

};

} // namespace SCION_EDITOR
