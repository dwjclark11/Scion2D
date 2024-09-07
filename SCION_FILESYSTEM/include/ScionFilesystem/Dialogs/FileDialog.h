#include <string>
#include <vector>

namespace SCION_FILESYSTEM
{
class FileDialog
{
  public:
	std::string OpenFileDialog( const std::string& sTitle = "Open", const std::string& sDefaultPath = "",
								const std::vector<const char*> filters = {}, const std::string& sFilterDesc = "" );

	std::string SaveFileDialog( const std::string& sTitle = "Save", const std::string& sDefaultPath = "",
								const std::vector<const char*> filters = {}, const std::string& sFilterDesc = "" );

	std::string SelectFolderDialog( const std::string& sTitle = "Select Folder", const std::string& sDefaultPath = "" );
};
} // namespace SCION_FILESYSTEM
