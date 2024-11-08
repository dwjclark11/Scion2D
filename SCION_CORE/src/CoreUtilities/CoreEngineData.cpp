#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/Scripting/UserDataBindings.h"

#include <Logger/Logger.h>

using namespace SCION_CORE::ECS;

namespace SCION_CORE
{
constexpr float METERS_TO_PIXELS = 12.f;
constexpr float PIXELS_TO_METERS = 1.f / METERS_TO_PIXELS;

CoreEngineData::CoreEngineData()
	: m_DeltaTime{ 0.f }
	, m_ScaledWidth{ 0.f }
	, m_ScaledHeight{ 0.f }
	, m_WindowWidth{ 640 }
	, m_WindowHeight{ 480 }
	, m_VelocityIterations{ 10 }
	, m_PositionIterations{ 8 }
	, m_bPhysicsEnabled{ true }
	, m_bPhysicsPaused{ false }
	, m_bRenderColliders{ false }
{
	m_ScaledWidth = m_WindowWidth / METERS_TO_PIXELS;
	m_ScaledHeight = m_WindowHeight / METERS_TO_PIXELS;
}

CoreEngineData& CoreEngineData::GetInstance()
{
	static CoreEngineData instance{};
	return instance;
}

void CoreEngineData::UpdateDeltaTime()
{
	auto now = std::chrono::steady_clock::now();
	m_DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>( now - m_LastUpdate ).count() / 1000000.0f;
	m_LastUpdate = now;
	// TODO: Fix Delta Time
}

void CoreEngineData::SetWindowWidth( int windowWidth )
{
	m_WindowWidth = windowWidth;
	m_ScaledWidth = m_WindowWidth / METERS_TO_PIXELS;
}

void CoreEngineData::SetWindowHeight( int windowHeight )
{
	m_WindowHeight = windowHeight;
	m_ScaledHeight = m_WindowHeight / METERS_TO_PIXELS;
}

const float CoreEngineData::MetersToPixels() const
{
	return METERS_TO_PIXELS;
}

const float CoreEngineData::PixelsToMeters() const
{
	return PIXELS_TO_METERS;
}

void CoreEngineData::RegisterMetaFunctions()
{
	Entity::RegisterMetaComponent<Identification>();
	Entity::RegisterMetaComponent<TransformComponent>();
	Entity::RegisterMetaComponent<SpriteComponent>();
	Entity::RegisterMetaComponent<AnimationComponent>();
	Entity::RegisterMetaComponent<BoxColliderComponent>();
	Entity::RegisterMetaComponent<CircleColliderComponent>();
	Entity::RegisterMetaComponent<PhysicsComponent>();
	Entity::RegisterMetaComponent<TextComponent>();
	Entity::RegisterMetaComponent<RigidBodyComponent>();

	Registry::RegisterMetaComponent<Identification>();
	Registry::RegisterMetaComponent<TransformComponent>();
	Registry::RegisterMetaComponent<SpriteComponent>();
	Registry::RegisterMetaComponent<AnimationComponent>();
	Registry::RegisterMetaComponent<BoxColliderComponent>();
	Registry::RegisterMetaComponent<CircleColliderComponent>();
	Registry::RegisterMetaComponent<PhysicsComponent>();
	Registry::RegisterMetaComponent<TextComponent>();
	Registry::RegisterMetaComponent<RigidBodyComponent>();

	// Register User Data Types
	SCION_CORE::Scripting::UserDataBinder::register_meta_user_data<ObjectData>();
}

} // namespace SCION_CORE
