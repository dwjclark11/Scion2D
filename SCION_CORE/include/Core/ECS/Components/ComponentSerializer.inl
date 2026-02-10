#include "ComponentSerializer.h"

namespace Scion::Core::ECS
{
template <typename TComponent, typename TSerializer>
inline void ComponentSerializer::Serialize( TSerializer& serializer, const TComponent& component )
{
	SerializeComponent( serializer, component );
}

template <typename TComponent, typename TTable>
inline void ComponentSerializer::Deserialize( const TTable& table, TComponent& component )
{
	DeserializeComponent( table, component );
}

} // namespace Scion::Core::ECS
