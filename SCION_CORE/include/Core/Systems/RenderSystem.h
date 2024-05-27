#pragma once
#include "Core/ECS/Registry.h"
#include <Rendering/Core/BatchRenderer.h>

namespace SCION_CORE::Systems {
	class RenderSystem
	{
	private: 
		SCION_CORE::ECS::Registry& m_Registry;
		std::unique_ptr<SCION_RENDERING::SpriteBatchRenderer> m_pBatchRenderer;

	public:
		RenderSystem(SCION_CORE::ECS::Registry& registry);
		~RenderSystem() = default;

		/*
		* @brief Loops through all of the entities in the registry that have a sprite
		* and transform component. Applies all the necessary transformations and adds them
		* to a Batch to be rendered.
		*/
		void Update();
	};
}