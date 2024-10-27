#include "SceneObject.h"
#include "fmt/format.h"

namespace SCION_EDITOR
{
SceneObject::SceneObject( const std::string& sceneName )
	: m_Registry{}
	, m_RuntimeRegistry{}
	, m_sSceneName{ sceneName }
	, m_Canvas{}
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
	static int number{ 0 };
	m_LayerParams.emplace_back( SCION_UTIL::SpriteLayerParams{ .sLayerName = fmt::format( "NewLayer_{}", number++ ) } );
}

} // namespace SCION_EDITOR
