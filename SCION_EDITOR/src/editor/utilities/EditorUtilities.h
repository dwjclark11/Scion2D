#pragma once
#include "Core/ECS/Components/AllComponents.h"
#include "Physics/UserData.h"

namespace SCION_EDITOR
{
constexpr const char* DROP_TEXTURE_SRC = "DropTextureSource";
constexpr const char* DROP_FONT_SRC = "DropFontSource";
constexpr const char* DROP_SOUNDFX_SRC = "DropSoundFxSource";
constexpr const char* DROP_MUSIC_SRC = "DropMusicSource";
constexpr const char* DROP_SCENE_SRC = "DropSceneSource";

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

} // namespace SCION_EDITOR
