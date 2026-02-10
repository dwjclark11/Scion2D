#pragma once
#include "Core/ECS/Components/AllComponents.h"
#include "Physics/UserData.h"

namespace Scion::Rendering
{
class Texture;
}

namespace Scion::Core
{
class ProjectInfo;
}

namespace Scion::Editor
{
constexpr const std::string_view DROP_TEXTURE_SRC = "DropTextureSource";
constexpr const std::string_view DROP_FONT_SRC = "DropFontSource";
constexpr const std::string_view DROP_SOUNDFX_SRC = "DropSoundFxSource";
constexpr const std::string_view DROP_MUSIC_SRC = "DropMusicSource";
constexpr const std::string_view DROP_SCENE_SRC = "DropSceneSource";
constexpr const std::string_view DROP_PREFAB_SRC = "DropPrefabSource";

struct Tile
{
	uint32_t id{ entt::null };
	Scion::Core::ECS::TransformComponent transform{};
	Scion::Core::ECS::SpriteComponent sprite{};
	Scion::Core::ECS::AnimationComponent animation{};
	Scion::Core::ECS::BoxColliderComponent boxCollider{};
	Scion::Core::ECS::CircleColliderComponent circleCollider{};
	Scion::Core::ECS::PhysicsComponent physics{};
	Scion::Physics::ObjectData objectData{};

	bool bCollider{ false }, bAnimation{ false }, bPhysics{ false }, bCircle{ false };
};

constexpr Scion::Rendering::Color XAXIS_GIZMO_COLOR = { 255, 0, 0, 175 };
constexpr Scion::Rendering::Color XAXIS_HOVERED_GIZMO_COLOR = { 255, 255, 0, 175 };
constexpr Scion::Rendering::Color YAXIS_GIZMO_COLOR = { 0, 255, 0, 175 };
constexpr Scion::Rendering::Color YAXIS_HOVERED_GIZMO_COLOR = { 0, 255, 255, 175 };

constexpr Scion::Rendering::Color GRAYED_OUT_GIZMO_COLOR = { 135, 135, 135, 175 };

struct MouseGuiInfo
{
	glm::vec2 position{ 0.f };
	glm::vec2 windowSize{ 0.f };
};

struct GizmoAxisParams
{
	Scion::Core::ECS::TransformComponent transform{};
	Scion::Core::ECS::SpriteComponent sprite{};
	glm::vec2 axisOffset;

	Scion::Rendering::Color axisColor;
	Scion::Rendering::Color axisHoveredColor;
	Scion::Rendering::Color axisDisabledColor;
};

/* Supported file types */
enum class EFileType
{
	SOUND,
	IMAGE,
	TXT, // All text file types
	FOLDER,
	INVALID_TYPE,
};

EFileType GetFileType( const std::string& sPath );

std::vector<std::string> SplitStr( const std::string& str, char delimiter );
Scion::Rendering::Texture* GetIconTexture( const std::string& sPath );
bool IsReservedPathOrFile( const std::filesystem::path& path );
bool IsDefaultProjectPathOrFile( const std::filesystem::path& path, const Scion::Core::ProjectInfo& projectInfo );

} // namespace Scion::Editor
