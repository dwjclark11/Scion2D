#include "CreateTileToolCmds.h"

#include "Core/ECS/Registry.h"
#include "Core/ECS/Entity.h"
#include "Core/ECS/Components/AllComponents.h"

#include "editor/utilities/EditorUtilities.h"
#include "Logger/Logger.h"

namespace SCION_EDITOR
{
void CreateTileToolAddCmd::undo()
{
	SCION_LOG( "Undone Create Tile Tool Cmd" );
}

void CreateTileToolAddCmd::redo()
{
	SCION_LOG( "Redone Create Tile Tool Cmd" );
}

} // namespace SCION_EDITOR
