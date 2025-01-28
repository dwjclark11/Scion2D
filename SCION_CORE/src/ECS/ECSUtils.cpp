#include "Core/ECS/ECSUtils.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/Entity.h"

#include <ranges>

namespace SCION_CORE::ECS
{

entt::entity FindEntityByTag( Registry& registry, const std::string& sTag )
{
	auto ids = registry.GetRegistry().view<Identification>( entt::exclude<TileComponent> );

	auto parItr = std::ranges::find_if( ids, [ & ]( const auto& e ) {
		Entity en{ registry, e };
		return en.GetName() == sTag;
	} );

	if ( parItr != ids.end() )
	{
		return *parItr;
	}

	return entt::entity{ entt::null };
}

} // namespace SCION_CORE::ECS
