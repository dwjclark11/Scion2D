#include "Core/ECS/MetaUtilities.h"

entt::id_type SCION_CORE::Utils::GetIdType( const sol::table& comp )
{
	if ( !comp.valid() )
	{
		SCION_ERROR( "Failed to get the type id -- Component has not been exposed to lua!" );
		assert( comp.valid() && "Failed to get the type id -- Component has not been exposed to lua!" );
		return -1;
	}

	const auto func = comp[ "type_id" ].get<sol::function>();
	assert( func.valid() && "[type_id()] - function has not been exposed to lua!"
							"\nPlease ensure all components and types have a type_id function"
							"\nwhen creating the new usertype" );

	return func.valid() ? func().get<entt::id_type>() : -1;
}
