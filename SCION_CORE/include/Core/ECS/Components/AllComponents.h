#pragma once
#include "BoxColliderComponent.h"
#include "CircleColliderComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "PhysicsComponent.h"
#include "RigidBodyComponent.h"
#include "ScriptComponent.h"
#include "Identification.h"
#include "TextComponent.h"
#include "TileComponent.h"
#include "Relationship.h"
#include "UIComponent.h"
#include "PersistentComponent.h"

namespace SCION_CORE::ECS
{
enum class EUneditableType
{
	PlayerStart,
	/* TODO: add more uneditable types as needed. */
	NoType
};
/*
 * UneditableComponent
 */

struct UneditableComponent
{
	EUneditableType eType{ EUneditableType::PlayerStart };
};

} // namespace SCION_CORE::ECS
