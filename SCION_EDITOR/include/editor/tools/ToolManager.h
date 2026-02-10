#pragma once
#include <entt/entt.hpp>

namespace Scion::Rendering
{
class Camera2D;
}

namespace Scion::Core::Events
{
class EventDispatcher;
}

namespace Scion::Core
{
struct Canvas;
}

namespace Scion::Editor
{

class AbstractTool;
class TileTool;
class Gizmo;
class SceneObject;
enum class EToolType;
enum class EGizmoType;

class ToolManager
{
  public:
	ToolManager();
	~ToolManager() = default;

	void Update( Scion::Core::Canvas& canvas );

	/*
	 * @brief Activates the tool based on the passed in tool type.
	 * This will also deactivate all other tools and gizmos.
	 * @param The tool type we want to set active.
	 */
	void SetToolActive( EToolType eToolType );

	/*
	 * @brief Activates the gizmo based on the passed in gizmo type.
	 * This will also deactivate all other tools and gizmos.
	 * @param The gizmo type we want to set active.
	 */
	void SetGizmoActive( EGizmoType eGizmoType );

	/*
	 * @brief Gets the current activated tool.
	 * @return Returns a pointer to a TileTool if activated, else returns nullptr;
	 */
	TileTool* GetActiveTool();

	/*
	 * @brief Gets the current activated gizmo.
	 * @return Returns a pointer to a Gizmo if activated, else returns nullptr;
	 */
	Gizmo* GetActiveGizmo();

	/*
	 * @brief Gets the current activated abstract tool.
	 * @return Returns a pointer to a Abstract tool if activated, else returns nullptr;
	 */
	AbstractTool* GetActiveToolFromAbstract();

	/*
	 * @brief Calls SetupTool for all tools and gizmos
	 */
	bool SetupTools( SceneObject* pSceneObject, Scion::Rendering::Camera2D* pCamera );

	/*
	 * @brief Sets the passed in tileset for all tools.
	 */
	void SetToolsCurrentTileset( const std::string& sTileset );

	/*
	 * @brief Sets the Tile Tools Start sprite coords for all tile tools.
	 * @param Takes in an int for both the startX and startY positions.
	 */
	void SetTileToolStartCoords( int x, int y );

	inline EToolType GetActiveToolType() const { return m_eActiveToolType; }
	inline EGizmoType GetActiveGizmoType() const { return m_eActiveGizmoType; }

	const bool IsGridSnapEnabled();
	void SetSelectedEntity( entt::entity entity );
	void EnableGridSnap( bool bEnable );

	std::vector<Scion::Core::Events::EventDispatcher*> GetDispatchers();

  private:
	std::map<EToolType, std::unique_ptr<TileTool>> m_mapTools;
	std::map<EGizmoType, std::unique_ptr<Gizmo>> m_mapGizmos;

	EToolType m_eActiveToolType;
	EGizmoType m_eActiveGizmoType;
};
} // namespace Scion::Editor

