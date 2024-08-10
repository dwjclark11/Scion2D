#pragma once
#include <map>
#include <memory>

namespace SCION_EDITOR
{

struct Canvas;
class TileTool;
enum class EToolType;

class ToolManager
{
  private:
	std::map<EToolType, std::unique_ptr<TileTool>> m_mapTools;

  public:
	ToolManager();
	~ToolManager() = default;

	void Update( Canvas& canvas );

	/*
	 * @brief Activates the tool based on the passed in tool type.
	 * This will also deactivate all other tools.
	 * @param The tool type we want to set active.
	 */
	void SetToolActive( EToolType eToolType );

	/*
	 * @brief Gets the current activated tool.
	 * @return Returns a pointer to a TileTool if activated, else returns nullptr;
	 */
	TileTool* GetActiveTool();
};
} // namespace SCION_EDITOR