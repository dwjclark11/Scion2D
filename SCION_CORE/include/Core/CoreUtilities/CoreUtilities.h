#pragma once
#include "Core/ECS/Components/AllComponents.h"
#include <Rendering/Core/Camera2D.h>

namespace SCION_CORE
{

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

} // namespace SCION_CORE
