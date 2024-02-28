#pragma once
#include <Rendering/Core/Camera2D.h>
#include "CoreEngineData.h"
#include "../ECS/Entity.h"

namespace SCION_CORE {

	struct FollowCamParams
	{
		float minX{ 0.f }, minY{ 0.f }, scale{ 1.f }, springback{ 1.f };
		float maxX{ static_cast<float>(CoreEngineData::GetInstance().WindowWidth()) };
		float maxY{ static_cast<float>(CoreEngineData::GetInstance().WindowHeight()) };
	};

	class FollowCamera
	{
	private:
		SCION_RENDERING::Camera2D& m_Camera;
		FollowCamParams m_Params;
		ECS::Entity m_Entity;

	public:
		FollowCamera(SCION_RENDERING::Camera2D& camera, const ECS::Entity& entity, const FollowCamParams& params);
		~FollowCamera() = default;

		void Update();
		inline void SetCameraParameters(const FollowCamParams& params) { m_Params = params; }
		inline void SetEntity(const ECS::Entity& entity) { m_Entity = entity; }
		inline void SetSpringback(float springback) { m_Params.springback = springback; }
		inline const FollowCamParams& GetParams() const { return m_Params; }

		static void CreateLuaFollowCamera(sol::state& lua, ECS::Registry& registry);
	};
}