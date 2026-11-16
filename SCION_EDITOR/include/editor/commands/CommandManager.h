#pragma once
#include "AbstractCommandManager.h"
#include "cmds/CreateTileToolCmds.h"
#include "cmds/RectToolCmds.h"
#include "cmds/TileCmds.h"

namespace Scion::Editor
{
using UndoableCommands = VarCommands<CreateTileToolAddCmd, CreateTileToolRemoveCmd, RectToolAddTilesCmd,
									 RectToolRemoveTilesCmd, RemoveTileLayerCmd, AddTileLayerCmd,
									 ChangeTileLayerNameCmd, MoveTileLayerCmd /* Add other command types */>;

class CommandManager
	: public AbstractCommandManager<CreateTileToolAddCmd, CreateTileToolRemoveCmd, RectToolAddTilesCmd,
									RectToolRemoveTilesCmd, RemoveTileLayerCmd, AddTileLayerCmd, ChangeTileLayerNameCmd,
									MoveTileLayerCmd /* Add other command types */>
{
  public:
	~CommandManager() = default;
};

} // namespace Scion::Editor
