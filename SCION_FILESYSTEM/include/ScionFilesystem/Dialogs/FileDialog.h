#include <string>
#include <vector>

namespace Scion::Filesystem
{
class FileDialog
{
  public:
	/*
	 * @brief Open a file dialog to select a specific file.
	 * @param std::string for the title we want to give the dialog.
	 * @param std::string for the default path.
	 * @param std::vector for the types of files to filter ex: {"*.png", "*.jpg"}.
	 * @param std::string for the filter description.
	 * @return This returns an std::string for the filepath of the selected file to open.
	 */
	std::string OpenFileDialog( const std::string& sTitle = "Open", const std::string& sDefaultPath = "",
								const std::vector<const char*>& filters = {}, const std::string& sFilterDesc = "" );

	/*
	 * @brief Open a file dialog to select a specific file.
	 * @param std::string for the title we want to give the dialog.
	 * @param std::string for the default path.
	 * @param std::vector for the types of files to filter ex: {"*.png", "*.jpg"}.
	 * @param std::string for the filter description.
	 * @return This returns an std::string for the filepath of the selected file to save.
	 */
	std::string SaveFileDialog( const std::string& sTitle = "Save", const std::string& sDefaultPath = "",
								const std::vector<const char*>& filters = {}, const std::string& sFilterDesc = "" );

	/*
	 * @brief Open a file dialog to select a specific file.
	 * @param std::string for the title we want to give the dialog.
	 * @param std::string for the default path.
	 * @return This returns an std::string for the filepath of the selected folder.
	 */
	std::string SelectFolderDialog( const std::string& sTitle = "Select Folder", const std::string& sDefaultPath = "" );
};
} // namespace Scion::Filesystem
