#pragma once 
#include "Rendering/Buffers/Framebuffer.h"
#include <map>

namespace SCION_EDITOR
{
	enum class FramebufferType
	{
		TILEMAP, SCENE, NO_TYPE
	};

	struct EditorFramebuffers
	{
		std::map<FramebufferType, std::shared_ptr<SCION_RENDERING::Framebuffer>> mapFramebuffers;
	};
}