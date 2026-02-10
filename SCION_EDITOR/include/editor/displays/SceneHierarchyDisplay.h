#pragma once
#include "IDisplay.h"
#include <imgui.h>

namespace Scion::Core::ECS
{
class Entity;
}

namespace Scion::Editor::Events
{
struct SwitchEntityEvent;
struct AddComponentEvent;
}

namespace Scion::Core::Events
{
struct KeyEvent;
}

namespace Scion::Editor
{
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

	void OpenContext( class SceneObject* pCurrentScene );
	
  private:
	std::shared_ptr<Scion::Core::ECS::Entity> m_pSelectedEntity{ nullptr };
	ImGuiTextFilter m_TextFilter;
	bool m_bAddComponent{ false };
	bool m_bWindowActive{ false };
};
} // namespace Scion::Editor
