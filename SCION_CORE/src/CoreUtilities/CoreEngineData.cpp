#include "Core/CoreUtilities/CoreEngineData.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Registry.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/Scripting/UserDataBindings.h"

#include <Logger/Logger.h>

using namespace Scion::Core::ECS;

namespace Scion::Core
{
constexpr float METERS_TO_PIXELS = 12.f;
constexpr float PIXELS_TO_METERS = 1.f / METERS_TO_PIXELS;

// clang-format off
static std::unordered_map<EGameType, std::string> g_mapGameTypeToStr
{
	{ EGameType::TopDown,		"Top Down" },
	{ EGameType::Platformer,	"Platformer" },
	{ EGameType::Fighting,		"Fighting" },
	{ EGameType::Puzzle,		"Puzzle" },
	{ EGameType::Rougelike,		"Rougelike" },
	{ EGameType::NoType,		"No Type" }
};

static std::unordered_map< std::string, EGameType > g_mapStrToGameType
{
	{ "Top Down",				EGameType::TopDown },
	{ "Platformer" ,			EGameType::Platformer },
	{ "Fighting",				EGameType::Fighting },
	{ "Puzzle",					EGameType::Puzzle },
	{ "Rougelike",				EGameType::Rougelike },
	{ "No Type",				EGameType::NoType }
};

// clang-format on

CoreEngineData::CoreEngineData()
	: m_DeltaTime{ 0.f }
	, m_ScaledWidth{ 0.f }
	, m_ScaledHeight{ 0.f }
	, m_Gravity{ 9.8f }
	, m_WindowWidth{ 640 }
	, m_WindowHeight{ 480 }
	, m_VelocityIterations{ 10 }
	, m_PositionIterations{ 8 }
	, m_bPhysicsEnabled{ true }
	, m_bPhysicsPaused{ false }
	, m_bRenderColliders{ false }
	, m_bRenderAnimations{ false }
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
	m_DeltaTime = std::chrono::duration<double>( now - m_LastUpdate ).count();
	m_LastUpdate = now;
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

void CoreEngineData::SetScaledWidth( float newWidth )
{
	// TODO: Add some clamps
	m_ScaledWidth = newWidth / METERS_TO_PIXELS;
}

void CoreEngineData::SetScaledHeight( float newHeight )
{
	// TODO: Add some clamps
	m_ScaledHeight = newHeight / METERS_TO_PIXELS;
}

std::string CoreEngineData::GetGameTypeStr( EGameType eType )
{
	if (auto itr = g_mapGameTypeToStr.find(eType); itr != g_mapGameTypeToStr.end())
	{
		return itr->second;
	}

	return {};
}

EGameType CoreEngineData::GetGameTypeFromStr( const std::string& sType )
{
	if ( auto itr = g_mapStrToGameType.find( sType ); itr != g_mapStrToGameType.end() )
	{
		return itr->second;
	}

	return EGameType::NoType;
}

const std::unordered_map<EGameType, std::string>& CoreEngineData::GetGameTypesMap()
{
	return g_mapGameTypeToStr;
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
	Entity::RegisterMetaComponent<TileComponent>();
	Entity::RegisterMetaComponent<Relationship>();
	Entity::RegisterMetaComponent<UIComponent>();

	Registry::RegisterMetaComponent<Identification>();
	Registry::RegisterMetaComponent<TransformComponent>();
	Registry::RegisterMetaComponent<SpriteComponent>();
	Registry::RegisterMetaComponent<AnimationComponent>();
	Registry::RegisterMetaComponent<BoxColliderComponent>();
	Registry::RegisterMetaComponent<CircleColliderComponent>();
	Registry::RegisterMetaComponent<PhysicsComponent>();
	Registry::RegisterMetaComponent<TextComponent>();
	Registry::RegisterMetaComponent<RigidBodyComponent>();
	Registry::RegisterMetaComponent<TileComponent>();
	Registry::RegisterMetaComponent<Relationship>();
	Registry::RegisterMetaComponent<UIComponent>();

	// Register User Data Types
	Scion::Core::Scripting::UserDataBinder::register_meta_user_data<ObjectData>();
}

} // namespace Scion::Core
