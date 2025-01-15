#pragma once
#include <string>

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

} // namespace SCION_EDITOR::Events
