#pragma once
#include "../ECS/Registry.h"

namespace SCION_CORE::Systems {
	class AnimationSystem
	{
	private:
		SCION_CORE::ECS::Registry& m_Registry;
	public:
		AnimationSystem(SCION_CORE::ECS::Registry& registry);
		~AnimationSystem() = default;

		void Update();
	};
}