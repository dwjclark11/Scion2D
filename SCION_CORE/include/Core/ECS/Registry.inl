#include "Registry.h"

namespace SCION_CORE::ECS
{
template <typename TContext>
inline TContext Registry::AddToContext( TContext context )
{
	return m_pRegistry->ctx().emplace<TContext>( context );
}

template <typename TContext>
inline TContext& Registry::GetContext()
{
	return m_pRegistry->ctx().get<TContext>();
}

template <typename TContext>
inline TContext* Registry::TryGetContext()
{
	return m_pRegistry->ctx().find<TContext>();
}

template <typename TContext>
bool Registry::RemoveContext()
{
	return m_pRegistry->ctx().erase<TContext>();
}

template <typename TContext>
bool Registry::HasContext()
{
	return m_pRegistry->ctx().contains<TContext>();
}

template <typename... Excludes>
inline void Registry::DestroyEntities()
{
	auto view = m_pRegistry->view<entt::entity>( entt::exclude<Excludes ...> );
	for ( auto entity : view )
	{
		m_pRegistry->destroy( entity );
	}
}

template <typename TComponent>
entt::runtime_view& add_component_to_view( Registry* registry, entt::runtime_view& view )
{
	return view.iterate( registry->GetRegistry().storage<TComponent>() );
}

template <typename TComponent>
auto exclude_component_from_view( Registry* registry, entt::runtime_view* view )
{
	view->exclude( registry->GetRegistry().storage<TComponent>() );
}

template <typename TComponent>
void Registry::RegisterMetaComponent()
{
	using namespace entt::literals;
	entt::meta<TComponent>()
		.type( entt::type_hash<TComponent>::value() )
		.template func<&add_component_to_view<TComponent>>( "add_component_to_view"_hs )
		.template func<&exclude_component_from_view<TComponent>>( "exclude_component_from_view"_hs );
}
} // namespace SCION_CORE::ECS
