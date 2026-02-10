#pragma once
#include "Rendering/Buffers/Framebuffer.h"

namespace Scion::Editor
{
enum class FramebufferType
{
	TILEMAP,
	SCENE,
	NO_TYPE
};

struct EditorFramebuffers
{
	std::map<FramebufferType, std::shared_ptr<Scion::Rendering::Framebuffer>> mapFramebuffers;
};
} // namespace Scion::Editor
