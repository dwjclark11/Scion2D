#pragma once
#include "IDisplay.h"

namespace Scion::Editor::Events
{
enum class EFileAction;
enum class EContentCreateAction;
struct FileEvent;
struct ContentCreateEvent;
struct KeyPressedEvent;
} // namespace Scion::Editor::Events

namespace Scion::Core::Events
{
class EventDispatcher;
}

namespace Scion::Editor
{
class ContentDisplay : public IDisplay
{
  public:
	ContentDisplay();
	virtual ~ContentDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	virtual void DrawToolbar() override;

	void CopyDroppedFile( const std::string& sFileToCopy, const std::filesystem::path& droppedPath );
	void MoveFolderOrFile( const std::filesystem::path& movedPath, const std::filesystem::path& path );
	void HandleFileEvent( const Scion::Editor::Events::FileEvent& fileEvent );
	void HandleCreateEvent( const Scion::Editor::Events::ContentCreateEvent& createEvent );
	void HandlePopups();

	void OpenDeletePopup();
	void OpenCreateFolderPopup();

	void OpenCreateLuaClassPopup();
	void OpenCreateLuaStateClassPopup();
	void OpenCreateLuaTablePopup();
	void OpenCreateEmptyLuaFilePopup();

  private:
	std::unique_ptr<Scion::Core::Events::EventDispatcher> m_pFileDispatcher;
	std::filesystem::path m_CurrentDir;
	std::string m_sFilepathToAction;
	int m_Selected;

	Scion::Editor::Events::EFileAction m_eFileAction;
	Scion::Editor::Events::EContentCreateAction m_eCreateAction;

	bool m_bItemCut;
	bool m_bWindowHovered;

};
} // namespace Scion::Editor
