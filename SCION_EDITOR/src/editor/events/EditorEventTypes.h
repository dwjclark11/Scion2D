#pragma once
#include <string>

namespace SCION_CORE::ECS
{
class Entity;
}

namespace SCION_EDITOR::Events
{
enum class EFileAction
{
	Delete,
	Copy,
	Cut,
	Paste,
	Rename,
	FileDropped,
	NoAction
};

struct FileEvent
{
	EFileAction eAction{ EFileAction::NoAction };
	std::string sFilepath{ "" };
};

struct CloseEditorEvent
{
	//
};

struct SwitchEntityEvent
{
	SCION_CORE::ECS::Entity* pEntity{ nullptr };
};

} // namespace SCION_EDITOR::Events
