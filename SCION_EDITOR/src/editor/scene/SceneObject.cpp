#include "SceneObject.h"
#include "ScionUtilities/ScionUtilities.h"

#include <fmt/format.h>

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
}

void SceneObject::ClearRuntimeScene()
{
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
