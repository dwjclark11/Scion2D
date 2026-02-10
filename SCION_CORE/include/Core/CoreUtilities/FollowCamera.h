#pragma once
#include <Rendering/Core/Camera2D.h>
#include "CoreEngineData.h"
#include "Core/ECS/Entity.h"

namespace Scion::Core
{

struct FollowCamParams
{
	float minX{ 0.f };
	float minY{ 0.f };
	float scale{ 1.f };
	float springback{ 1.f };
	float maxX{ static_cast<float>( CoreEngineData::GetInstance().WindowWidth() ) };
	float maxY{ static_cast<float>( CoreEngineData::GetInstance().WindowHeight() ) };
};

class FollowCamera
{
  private:
	Scion::Rendering::Camera2D& m_Camera;
	FollowCamParams m_Params;
	ECS::Entity m_Entity;

  public:
	FollowCamera( Scion::Rendering::Camera2D& camera, const ECS::Entity& entity, const FollowCamParams& params );
	~FollowCamera() = default;

	void Update();
	void SetSpringback( float springback );
	void SetCameraParameters( const FollowCamParams& params );

	inline void SetEntity( const ECS::Entity& entity ) { m_Entity = entity; }
	inline const FollowCamParams& GetParams() const { return m_Params; }

	static void CreateLuaFollowCamera( sol::state& lua, ECS::Registry& registry );
};
} // namespace Scion::Core
