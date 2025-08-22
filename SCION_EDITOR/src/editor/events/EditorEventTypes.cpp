#include "EditorEventTypes.h"

namespace SCION_EDITOR::Events
{

// clang-format off
static std::map<EComponentType, std::string> g_mapComponentToStr
{
	{ EComponentType::Transform, "TransformComponent" },
	{ EComponentType::Sprite, "SpriteComponent" },
	{ EComponentType::Animation, "AnimationComponent" },
	{ EComponentType::Text, "TextComponent" },
	{ EComponentType::UI, "UIComponent" },
	{ EComponentType::Physics, "PhysicsComponent" },
	{ EComponentType::RigidBody, "RigidBodyComponent" },
	{ EComponentType::BoxCollider, "BoxColliderComponent" },
	{ EComponentType::CircleCollider, "CircleColliderComponent" },
	{ EComponentType::Tile, "TileComponent" }
};
// clang-format on

EComponentType GetComponentTypeFromStr( const std::string& componentStr )
{
	auto itr =
		std::ranges::find_if( g_mapComponentToStr, [ & ]( const auto& pair ) { return pair.second == componentStr; } );
	if ( itr == g_mapComponentToStr.end() )
		return EComponentType::NoType;

	return itr->first;
}

std::string GetComponentStrFromType( EComponentType eType )
{
	auto itr = g_mapComponentToStr.find( eType );
	if ( itr == g_mapComponentToStr.end() )
		return {};

	return itr->second;
}

} // namespace SCION_EDITOR::Events
