#include "Core/ECS/Registry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/MetaUtilities.h"
#include "Core/ECS/ECSUtils.h"

using namespace SCION_CORE::Utils;

SCION_CORE::ECS::Registry::Registry()
	: m_pRegistry{ std::make_shared<entt::registry>() }
{
}

void SCION_CORE::ECS::Registry::CreateLuaRegistryBind( sol::state& lua, Registry& registry )
{
	lua.new_enum<ERegistryType>( "RegistryType",
								 {
									 { "Lua", ERegistryType::LuaRegistry },
									 { "Scion", ERegistryType::ScionRegistry },
								 } );

	using namespace entt::literals;

	lua.new_usertype<entt::runtime_view>(
		"runtime_view",
		sol::no_constructor,
		"for_each",
		sol::overload(
			[ & ]( const entt::runtime_view& view, const sol::function& callback, sol::this_state s ) {
				if ( !callback.valid() )
					return;

				for ( auto entity : view )
				{
					Entity ent{ &registry, entity };
					callback( ent );
				}
			},
			[]( const entt::runtime_view& view, Registry& reg, const sol::function& callback, sol::this_state s ) {
				if ( !callback.valid() )
					return;

				for ( auto entity : view )
				{
					Entity ent{ &reg, entity };
					callback( ent );
				}
			} ),
		"exclude",
		sol::overload(
			[ & ]( entt::runtime_view& view, const sol::variadic_args& va ) {
				for ( const auto& type : va )
				{
					if ( !type.as<sol::table>().valid() )
						continue;

					const auto excluded_view =
						InvokeMetaFunction( GetIdType( type ), "exclude_component_from_view"_hs, &registry, view );

					view = excluded_view ? excluded_view.cast<entt::runtime_view>() : view;
				}
			},
			[ & ]( entt::runtime_view& view, Registry& reg, const sol::variadic_args& va ) {
				for ( const auto& type : va )
				{
					if ( !type.as<sol::table>().valid() )
						continue;

					const auto excluded_view =
						InvokeMetaFunction( GetIdType( type ), "exclude_component_from_view"_hs, &reg, view );

					view = excluded_view ? excluded_view.cast<entt::runtime_view>() : view;
				}
			} ) );

	lua.new_usertype<Registry>(
		"Registry",
		sol::call_constructor,
		sol::factories( [ & ]( sol::this_state s ) { return sol::make_reference( s, std::ref( registry ) ); },
						[]( ERegistryType eType, sol::this_state s ) {
							return eType != ERegistryType::LuaRegistry ? sol::lua_nil_t{}
																	   : sol::make_object( s, Registry{} );
						} ),
		"getEntities",
		[ & ]( Registry& reg, const sol::variadic_args& va ) {
			entt::runtime_view view{};
			for ( const auto& type : va )
			{
				if ( !type.as<sol::table>().valid() )
					continue;

				const auto entities = InvokeMetaFunction( GetIdType( type ), "add_component_to_view"_hs, &reg, view );

				view = entities ? entities.cast<entt::runtime_view>() : view;
			}

			return view;
		},
		"findEntityByTag",
		[ & ]( Registry& reg, const std::string& sTag, sol::this_state s ) {
			auto entity = SCION_CORE::ECS::FindEntityByTag( reg, sTag );

			return entity == entt::null ? sol::lua_nil_t{} : sol::make_reference( s, Entity{ &reg, entity } );
		},
		"createEntity",
		sol::overload( []( Registry& reg ) { return Entity{ &reg, "", "" }; },
					   []( Registry& reg, const std::string& sName, const std::string sGroup ) {
						   return Entity{ &reg, sName, sGroup };
					   } ),
		"clear",
		[ & ]( Registry& reg ) { reg.GetRegistry().clear(); } );
}
