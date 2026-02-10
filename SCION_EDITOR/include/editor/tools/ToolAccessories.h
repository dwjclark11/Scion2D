#pragma once

namespace Scion::Editor
{
enum class EToolType
{
	CREATE_TILE = 0,
	RECT_FILL_TILE,

	NO_TOOL
};

enum class EGizmoType
{
	TRANSLATE = 0,
	SCALE,
	ROTATE,

	NO_GIZMO
};

} // namespace Scion::Editor
