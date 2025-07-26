#pragma once
#include "Core/ECS/Components/AllComponents.h"
#include <Rendering/Core/Camera2D.h>

namespace SCION_RESOURCES
{
class AssetManager;
}

namespace SCION_CORE
{

namespace ECS
{
class Registry;
class Entity;
}

/**
 * @brief Checks if an entity is fully within the camera's view before culling.
 *
 * This function performs an **axis-aligned bounding box (AABB) check** to determine
 * if an entity is **completely out of view** before marking it for culling.
 * It ensures that objects are not removed too early when only partially outside
 * the viewport.
 * @param transform The entity's transform component (position, scale).
 * @param width The entity's width in world space.
 * @param height The entity's height in world space.
 * @param camera The camera used for visibility checking.
 * @return True if the entity is at least partially visible, false if fully out of view.
 */
bool EntityInView( const SCION_CORE::ECS::TransformComponent& transform, float width, float height,
				   const SCION_RENDERING::Camera2D& camera );

/**
 * @brief Constructs an RST (Rotation, Scale, Translation) transformation matrix.
 *
 * Generates a transformation matrix based on the given position, rotation, and scale,
 * applying translation, rotation around the Z-axis, and scaling.
 *
 * @param transform The transform component containing position, rotation, and scale.
 * @param width The object's width, used for pivot adjustments.
 * @param height The object's height, used for pivot adjustments.
 * @return The computed transformation matrix.
 */
glm::mat4 RSTModel( const SCION_CORE::ECS::TransformComponent& transform, float width, float height );

/**
 * @brief Generates UV coordinates for a sprite based on its dimensions and texture size.
 *
 * Computes normalized UV coordinates by mapping the sprite's position and size within
 * the texture.
 *
 * @param sprite The sprite component to update with UV coordinates.
 * @param textureWidth The width of the texture.
 * @param textureHeight The height of the texture.
 */
void GenerateUVs( SCION_CORE::ECS::SpriteComponent& sprite, int textureWidth, int textureHeight );

void GenerateUVsExt( SCION_CORE::ECS::SpriteComponent& sprite, int textureWidth, int textureHeight, float u, float v );

/**
 * @brief Converts world coordinates to isometric grid coordinates.
 *
 * Applies transformations to map a world position to an isometric tile grid,
 * considering tile size and rotation.
 *
 * @param position The world position to convert.
 * @param canvas The canvas defining tile dimensions.
 * @return A tuple containing the isometric grid coordinates (x, y).
 */
std::tuple<int, int> ConvertWorldPosToIsoCoords( const glm::vec2& position, const struct Canvas& canvas );

/**
 * @brief Returns the pixel size of a text block based on font metrics and wrapping settings.
 *
 * Calculates the width and height required to render the text from a TextComponent,
 * using font data from the AssetManager and optional word wrapping.
 *
 * @param textComp Text data including string, font, and wrap width.
 * @param transform Starting position for text rendering.
 * @param assetManager Asset manager used to retrieve the font.
 * @return A tuple (width, height) representing the text block size in pixels.
 */
std::tuple<float, float> GetTextBlockSize( const SCION_CORE::ECS::TextComponent& textComp,
										   const SCION_CORE::ECS::TransformComponent& transform,
										   SCION_RESOURCES::AssetManager& assetManager );

/**
 * @brief Resets the dirty flags on all necessary components in the registry.
 * Marks updated entities as clean by clearing their `bDirty` flags.
 */
void UpdateDirtyEntities( SCION_CORE::ECS::Registry& registry );

/* Target time per frame. Used to help clamp delta time. */
constexpr double TARGET_FRAME_TIME = 1.0 / 60.0;
/* Target time per frame. Used for Box2D step. */
constexpr float TARGET_FRAME_TIME_F = 1.0f / 60.0f;
/* Used to prevent specific loops from looping forever. */
constexpr int SANITY_LOOP_CHECK = 100;

struct GameObjectRelationship
{
	std::string sSelf{};
	std::string sParent{};
	std::string sNextSibling{};
	std::string sPrevSibling{};
	std::string sFirstChild{};
};

struct GameObjectData
{
	std::optional<GameObjectRelationship> relationship{ std::nullopt };
	std::optional<SCION_CORE::ECS::Identification> id{ std::nullopt };
	std::optional<SCION_CORE::ECS::TransformComponent> transform{ std::nullopt };
	std::optional<SCION_CORE::ECS::SpriteComponent> sprite{ std::nullopt };
	std::optional<SCION_CORE::ECS::AnimationComponent> animation{ std::nullopt };
	std::optional<SCION_CORE::ECS::BoxColliderComponent> boxCollider{ std::nullopt };
	std::optional<SCION_CORE::ECS::CircleColliderComponent> circleCollider{ std::nullopt };
	std::optional<SCION_CORE::ECS::PhysicsComponent> physics{ std::nullopt };
	std::optional<SCION_CORE::ECS::TextComponent> text{ std::nullopt };
	std::optional<SCION_CORE::ECS::UIComponent> ui{ std::nullopt };
	std::optional<SCION_PHYSICS::ObjectData> objectData{ std::nullopt };
};

GameObjectData GetGameObejectDataFromEntity( SCION_CORE::ECS::Entity & entity );

} // namespace SCION_CORE
