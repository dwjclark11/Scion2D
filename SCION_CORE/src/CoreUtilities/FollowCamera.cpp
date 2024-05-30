#include "Core/CoreUtilities/FollowCamera.h"
#include "Core/ECS/Components/TransformComponent.h"
#include <Logger/Logger.h>

using namespace SCION_CORE::ECS;

namespace SCION_CORE
{

FollowCamera::FollowCamera(SCION_RENDERING::Camera2D& camera, const ECS::Entity& entity, const FollowCamParams& params)
	: m_Camera{camera}
	, m_Params{params}
	, m_Entity{entity}
{
	SCION_ASSERT(m_Params.scale > 0.f && "The scale must be greater than zero!");

	m_Params.minX *= m_Params.scale;
	m_Params.minY *= m_Params.scale;
	m_Params.maxX *= m_Params.scale;
	m_Params.maxY *= m_Params.scale;

	m_Params.springback = std::clamp(m_Params.springback, 0.f, 1.f);
}

void FollowCamera::Update()
{
	const auto& transform = m_Entity.GetComponent<TransformComponent>();
	const auto& camWidth = m_Camera.GetWidth();
	const auto& camHeight = m_Camera.GetHeight();
	const auto& camPos = m_Camera.GetPosition();

	glm::vec2 newCamPosition{0.f};

	newCamPosition.x = (transform.position.x - camWidth / (2 * m_Params.scale)) * m_Params.scale;
	newCamPosition.y = (transform.position.y - camHeight / (2 * m_Params.scale)) * m_Params.scale;

	newCamPosition.x = std::clamp(newCamPosition.x, m_Params.minX, m_Params.maxX - camWidth);
	newCamPosition.y = std::clamp(newCamPosition.y, m_Params.minY, m_Params.maxY - camHeight);

	m_Camera.SetPosition(glm::vec2{std::lerp(camPos.x, newCamPosition.x, m_Params.springback),
								   std::lerp(camPos.y, newCamPosition.y, m_Params.springback)});
}

void FollowCamera::SetSpringback(float springback)
{
	m_Params.springback = std::clamp(springback, 0.f, 1.f);
}

void FollowCamera::SetCameraParameters(const FollowCamParams& params)
{
	m_Params = params;

	if (m_Params.scale <= 0.f)
		m_Params.scale = 0.1f;

	if (m_Params.springback < 0.f)
		m_Params.springback = 0.f;

	m_Params.minX *= m_Params.scale;
	m_Params.minY *= m_Params.scale;
	m_Params.maxX *= m_Params.scale;
	m_Params.maxY *= m_Params.scale;
}

void FollowCamera::CreateLuaFollowCamera(sol::state& lua, ECS::Registry& registry)
{
	lua.new_usertype<FollowCamParams>(
		"FollowCamParams",
		sol::call_constructor,
		sol::factories(
			[](const sol::table& params) {
				return FollowCamParams{
					.minX = params[ "min_x" ].get_or(0.f),
					.minY = params[ "min_y" ].get_or(0.f),
					.scale = params[ "scale" ].get_or(1.f),
					.springback = params[ "springback" ].get_or(1.f),
					.maxX = params[ "max_x" ].get_or(static_cast<float>(CoreEngineData::GetInstance().WindowWidth())),
					.maxY = params[ "max_y" ].get_or(static_cast<float>(CoreEngineData::GetInstance().WindowHeight()))};
			},
			[](float minX, float minY, float maxX, float maxY, float scale, float springback) {
				return FollowCamParams{
					.minX = minX, .minY = minY, .scale = scale, .springback = springback, .maxX = maxX, .maxY = maxY};
			}),
		"min_x",
		&FollowCamParams::minX,
		"min_y",
		&FollowCamParams::minY,
		"max_x",
		&FollowCamParams::maxX,
		"max_y",
		&FollowCamParams::maxY,
		"scale",
		&FollowCamParams::scale,
		"springback",
		&FollowCamParams::springback);

	auto& camera = registry.GetContext<std::shared_ptr<SCION_RENDERING::Camera2D>>();

	lua.new_usertype<FollowCamera>("FollowCamera",
								   sol::call_constructor,
								   sol::factories([ & ](const FollowCamParams& params, const ECS::Entity& entity) {
									   return FollowCamera{*camera, entity, params};
								   }),
								   "update",
								   &FollowCamera::Update,
								   "set_params",
								   &FollowCamera::SetCameraParameters,
								   "set_entity",
								   &FollowCamera::SetEntity,
								   "set_springback",
								   &FollowCamera::SetSpringback,
								   "get_params",
								   &FollowCamera::GetParams);
}

} // namespace SCION_CORE