#pragma once
#include <map>
#include <memory>
#include <string>

namespace SCION_RENDERING
{
class Camera2D;
}

namespace SCION_EDITOR
{

struct Canvas;
class TileTool;
class SceneObject;
enum class EToolType;
enum class EGizmoType;

class ToolManager
{
  private:
	std::map<EToolType, std::unique_ptr<TileTool>> m_mapTools;

	EToolType m_eActiveToolType;
	EGizmoType m_eActiveGizmoType;

  public:
	ToolManager();
	~ToolManager() = default;

	void Update( Canvas& canvas );

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
	 * @brief Calls SetupTool for all tools and gizmos
	 */
	bool SetupTools( SceneObject* pSceneObject, SCION_RENDERING::Camera2D* pCamera );

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

	void EnableGridSnap( bool bEnable );
};
} // namespace SCION_EDITOR
