#pragma once
#include "Core/ECS/Components/AllComponents.h"
#include "Physics/UserData.h"

namespace SCION_RENDERING
{
class Texture;
}

namespace SCION_EDITOR
{
constexpr const char* DROP_TEXTURE_SRC = "DropTextureSource";
constexpr const char* DROP_FONT_SRC = "DropFontSource";
constexpr const char* DROP_SOUNDFX_SRC = "DropSoundFxSource";
constexpr const char* DROP_MUSIC_SRC = "DropMusicSource";
constexpr const char* DROP_SCENE_SRC = "DropSceneSource";

#define BASE_PATH                                                                                                      \
	std::string                                                                                                        \
	{                                                                                                                  \
		SDL_GetBasePath()                                                                                              \
	}

#ifdef _WIN32
constexpr char PATH_SEPARATOR = '\\';
#define DEFAULT_PROJECT_PATH BASE_PATH + "ScionProjects"
#else
constexpr char PATH_SEPARATOR = '/';
#define DEFAULT_PROJECT_PATH BASE_PATH + PATH_SEPARATOR + "ScionProjects"
#endif

#define SCRIPTS "scripts"
#define ASSETS "assets" 

constexpr const char* S2D_PRJ_FILE_EXT = ".s2dprj";

struct Canvas
{
	int width{ 640 }, height{ 480 }, tileWidth{ 16 }, tileHeight{ 16 };
};

struct Tile
{
	uint32_t id{ entt::null };
	SCION_CORE::ECS::TransformComponent transform{};
	SCION_CORE::ECS::SpriteComponent sprite{};
	SCION_CORE::ECS::AnimationComponent animation{};
	SCION_CORE::ECS::BoxColliderComponent boxCollider{};
	SCION_CORE::ECS::CircleColliderComponent circleCollider{};
	SCION_CORE::ECS::PhysicsComponent physics{};
	SCION_PHYSICS::ObjectData objectData{};

	bool bCollider{ false }, bAnimation{ false }, bPhysics{ false }, bCircle{ false };
};

constexpr SCION_RENDERING::Color XAXIS_GIZMO_COLOR = { 255, 0, 0, 175 };
constexpr SCION_RENDERING::Color XAXIS_HOVERED_GIZMO_COLOR = { 255, 255, 0, 175 };
constexpr SCION_RENDERING::Color YAXIS_GIZMO_COLOR = { 0, 255, 0, 175 };
constexpr SCION_RENDERING::Color YAXIS_HOVERED_GIZMO_COLOR = { 0, 255, 255, 175 };

constexpr SCION_RENDERING::Color GRAYED_OUT_GIZMO_COLOR = { 135, 135, 135, 175 };

struct GizmoAxisParams
{
	SCION_CORE::ECS::TransformComponent transform{};
	SCION_CORE::ECS::SpriteComponent sprite{};
	glm::vec2 axisOffset;

	SCION_RENDERING::Color axisColor;
	SCION_RENDERING::Color axisHoveredColor;
	SCION_RENDERING::Color axisDisabledColor;
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
SCION_RENDERING::Texture* GetIconTexture( const std::string& sPath );

} // namespace SCION_EDITOR
