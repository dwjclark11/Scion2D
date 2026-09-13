#pragma once
#include "IDisplay.h"
#include "editor/scene/SceneFolderManager.h"
#include <imgui.h>

namespace Scion::Core::ECS
{
class Entity;
}

namespace Scion::Editor::Events
{
struct SwitchEntityEvent;
struct AddComponentEvent;
} // namespace Scion::Editor::Events

namespace Scion::Core::Events
{
struct KeyEvent;
}

namespace Scion::Editor
{

class SceneObject;

class SceneHierarchyDisplay : public IDisplay
{
  public:
	SceneHierarchyDisplay();
	~SceneHierarchyDisplay();

	virtual void Update() override;
	virtual void Draw() override;

  private:
	bool OpenTreeNode( Scion::Core::ECS::Entity& entity );

	void AddComponent( Scion::Core::ECS::Entity& entity, bool* bAddComponent );
	void DrawGameObjectDetails();
	void DrawUneditableTypes();
	void DrawPlayerStart();
	void DrawEntityComponents();
	bool DeleteSelectedEntity();
	bool DuplicateSelectedEntity();

	void OnEntityChanged( Scion::Editor::Events::SwitchEntityEvent& swEntEvent );
	void OnKeyPressed( Scion::Core::Events::KeyEvent& keyEvent );
	void OnAddComponent( Scion::Editor::Events::AddComponentEvent& addCompEvent );

	// -- Scene Folder --
	/** @brief Draws all folders and entities under those folders. */
	void DrawFolder( SceneFolder& folder, SceneObject* pCurrentScene );
	/** @brief Inline folder renaming function. */
	void DrawFolderRenameInline( SceneFolderManager& folderManager, SceneFolder& folder );
	/** @brief Folder context for renaming and deleting folders, etc. */
	void OpenFolderContext( SceneFolder& folder, SceneObject* pCurrentScene );
	/** @brief Actually delete the folder if set. */
	void DeleteSelectedFolder();

	void OpenContext( class SceneObject* pCurrentScene );

  private:
	std::shared_ptr<Scion::Core::ECS::Entity> m_pSelectedEntity{ nullptr };
	ImGuiTextFilter m_TextFilter;
	bool m_bAddComponent{ false };
	bool m_bWindowActive{ false };

	// Folder State Variables
	SceneFolder::Id m_RenamingFolderId{ 0 };
	char m_RenameBuffer[ 128 ]{};

	std::optional<SceneFolder::Id> m_SelectedEntityFolderId{ std::nullopt };
	std::optional<SceneFolder::Id> m_FolderToDeleteId{ std::nullopt };
};
} // namespace Scion::Editor
