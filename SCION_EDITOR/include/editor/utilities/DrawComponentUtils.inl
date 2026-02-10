#include "DrawComponentUtils.h"
#include "Core/ECS/Entity.h"

namespace Scion::Editor
{

template <typename TComponent>
inline void DrawComponentsUtil::DrawEntityComponentInfo( Scion::Core::ECS::Entity& entity )
{
	auto& component = entity.GetComponent<TComponent>();
	DrawImGuiComponent( entity, component );
}

template <typename TComponent>
inline void DrawComponentsUtil::DrawComponentInfo( TComponent& component )
{
	DrawImGuiComponent( component );
}

template <typename TComponent>
inline void DrawComponentsUtil::DrawComponentInfo( Scion::Core::ECS::Entity& entity, TComponent& component )
{
	DrawImGuiComponent( entity, component );
}

template <typename TComponent>
inline void DrawComponentsUtil::RegisterUIComponent()
{
	using namespace entt::literals;

	entt::meta_factory<TComponent>()
		.type( entt::type_hash<TComponent>::value() )
		.template func<&DrawEntityComponentInfo<TComponent>>( "DrawEntityComponentInfo"_hs );
}

} // namespace Scion::Editor
