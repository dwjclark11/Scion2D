#include "Core/CoreUtilities/CoreUtilities.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/Scene/Scene.h"

#include "ScionUtilities/MathUtilities.h"

using namespace SCION_CORE::ECS;

namespace SCION_CORE
{

bool EntityInView( const TransformComponent& transform, float width, float height,
				   const SCION_RENDERING::Camera2D& camera )
{
	const glm::vec2 cameraPos = camera.GetPosition() - camera.GetScreenOffset();
	const int cameraWidth = camera.GetWidth();
	const int cameraHeight = camera.GetHeight();
	const float cameraScale = camera.GetScale();
	const float invCameraScale = 1.f / cameraScale;

	const float cameraLeft = ( cameraPos.x - ( width * transform.scale.x * cameraScale ) ) * invCameraScale;
	const float cameraRight = ( cameraPos.x + cameraWidth ) * invCameraScale;
	const float cameraTop = ( cameraPos.y - ( height * transform.scale.y * cameraScale ) ) * invCameraScale;
	const float cameraBottom = ( cameraPos.y + cameraHeight ) * invCameraScale;

	if ( ( transform.position.x <= cameraLeft || transform.position.x >= cameraRight ) ||
		 ( transform.position.y <= cameraTop || transform.position.y >= cameraBottom ) )
	{
		return false;
	}

	// Entity is at least partially in view
	return true;
}

glm::mat4 RSTModel( const TransformComponent& transform, float width, float height )
{
	glm::mat4 model{ 1.f };
	if ( transform.rotation > 0.f || transform.rotation < 0.f || transform.scale.x > 1.f || transform.scale.x < 1.f ||
		 transform.scale.y > 1.f || transform.scale.y < 1.f )
	{
		model = glm::translate( model, glm::vec3{ transform.position, 0.f } );
		model = glm::translate(
			model, glm::vec3{ ( width * transform.scale.x ) * 0.5f, ( height * transform.scale.y ) * 0.5f, 0.f } );
		model = glm::rotate( model, glm::radians( transform.rotation ), glm::vec3{ 0.f, 0.f, 1.f } );
		model = glm::translate(
			model, glm::vec3{ ( width * transform.scale.x ) * -0.5f, ( height * transform.scale.y ) * -0.5f, 0.f } );
		model = glm::scale( model, glm::vec3{ transform.scale, 1.0f } );
		model = glm::translate( model, glm::vec3{ -transform.position, 0.f } );
	}

	return model;
}

void GenerateUVs( SCION_CORE::ECS::SpriteComponent& sprite, int textureWidth, int textureHeight )
{
	sprite.uvs.uv_width = sprite.width / textureWidth;
	sprite.uvs.uv_height = sprite.height / textureHeight;

	sprite.uvs.u = sprite.start_x * sprite.uvs.uv_width;
	sprite.uvs.v = sprite.start_y * sprite.uvs.uv_height;
}

void GenerateUVsExt( SCION_CORE::ECS::SpriteComponent& sprite, int textureWidth, int textureHeight, float u, float v )
{
	sprite.uvs.uv_width = sprite.width / textureWidth;
	sprite.uvs.uv_height = sprite.height / textureHeight;

	sprite.uvs.u = u;
	sprite.uvs.v = v;
}

std::tuple<int, int> ConvertWorldPosToIsoCoords( const glm::vec2& position, const Canvas& canvas )
{
	// TODO: Enforce width being double height. Double width hack for now.
	float doubleWidth = canvas.tileWidth * 2.f;

	// Move the x position back the halfWidth of one tile.
	// We are not currently using any offset for we have a camera that can go into the negatives.
	float xPos = ( position.x /*- canvas.offset.x*/ ) - doubleWidth * 0.5f;
	// Stretch the Y position by 2 in the negative to make the tile a square. Allows for easier rotations.
	float yPos = position.y * -2.f;

	// Rotate both the x and the y positions by 45 degrees
	/*
	*	px = x * cs - y * sn;
		py = x * sn + y * cs;
	*/
	float px = xPos * cos( SCION_MATH::PIOver4 ) - yPos * sin( SCION_MATH::PIOver4 );
	float py = xPos * sin( SCION_MATH::PIOver4 ) + yPos * cos( SCION_MATH::PIOver4 );

	float diagonal = canvas.tileHeight * sqrt( 2.f );

	int cellX = static_cast<int>( px / diagonal );
	int cellY = static_cast<int>( -py / diagonal );

	return std::make_tuple( cellX, cellY );
}

} // namespace SCION_CORE
