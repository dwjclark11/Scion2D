#include "SceneObject.h"
#include "ScionUtilities/ScionUtilities.h"
#include "Core/ECS/Components/AllComponents.h"
#include "Core/ECS/MetaUtilities.h"
#include <fmt/format.h>

using namespace SCION_CORE::ECS;
using namespace entt::literals;

namespace SCION_EDITOR
{
SceneObject::SceneObject( const std::string& sceneName )
	: m_Registry{}
	, m_RuntimeRegistry{}
	, m_sSceneName{ sceneName }
	, m_Canvas{}
	, m_CurrentLayer{ 0 }
{
}

void SceneObject::CopySceneToRuntime()
{
	auto& registryToCopy = m_Registry.GetRegistry();
	
	for ( auto entityToCopy : registryToCopy.view<entt::entity>( entt::exclude<ScriptComponent> ) )
	{
		entt::entity newEntity = m_RuntimeRegistry.CreateEntity();

		// Copy the components of the entity to the new entity
		for ( auto&& [ id, storage ] : registryToCopy.storage() )
		{
			if ( !storage.contains( entityToCopy ) )
				continue;

			SCION_CORE::Utils::InvokeMetaFunction(
				id, "copy_component"_hs, Entity{ m_Registry, entityToCopy }, Entity{ m_RuntimeRegistry, newEntity } );
		}
	}
}

void SceneObject::ClearRuntimeScene()
{
	m_RuntimeRegistry.ClearRegistry();
}

void SceneObject::AddNewLayer()
{
	m_LayerParams.emplace_back(
		SCION_UTIL::SpriteLayerParams{ .sLayerName = fmt::format( "NewLayer_{}", m_CurrentLayer++ ) } );
}

bool SceneObject::CheckLayerName( const std::string& sLayerName )
{
	return SCION_UTIL::CheckContainsValue( m_LayerParams, [ & ]( SCION_UTIL::SpriteLayerParams& spriteLayer ) {
		return spriteLayer.sLayerName == sLayerName;
	} );
}

} // namespace SCION_EDITOR
