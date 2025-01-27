#pragma once
#include "IDisplay.h"
#include <imgui.h>

namespace SCION_CORE::ECS
{
class Entity;
}

namespace SCION_EDITOR::Events
{
struct SwitchEntityEvent;
struct KeyPressedEvent;
}

namespace SCION_EDITOR
{
class SceneHierarchyDisplay : public IDisplay
{
  private:
	std::shared_ptr<SCION_CORE::ECS::Entity> m_pSelectedEntity{ nullptr };
	ImGuiTextFilter m_TextFilter;
	bool m_bAddComponent{ false };
	bool m_bWindowActive{ false };

  private:
	bool OpenTreeNode( SCION_CORE::ECS::Entity& entity );

	void AddComponent( SCION_CORE::ECS::Entity& entity, bool* bAddComponent );
	void DrawGameObjectDetails();
	void DrawEntityComponents();
	bool DeleteSelectedEntity();
	bool DuplicateSelectedEntity();

	void OnEntityChanged( SCION_EDITOR::Events::SwitchEntityEvent& swEntEvent );
	void OnKeyPressed( SCION_EDITOR::Events::KeyPressedEvent& keyPressed );

  public:
	SceneHierarchyDisplay();
	~SceneHierarchyDisplay();

	virtual void Update() override;
	virtual void Draw() override;
};
} // namespace SCION_EDITOR
