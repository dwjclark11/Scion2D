#pragma once
#include "AbstractCommandManager.h"
#include "cmds/CreateTileToolCmds.h"
#include "cmds/RectToolCmds.h"
#include "cmds/TileCmds.h"
#include "cmds/GameObjectCmds.h"
// TODO: add other commands as necessary

namespace SCION_EDITOR
{
using UndoableCommands =
	VarCommands<CreateTileToolAddCmd, CreateTileToolRemoveCmd, RectToolAddTilesCmd, RectToolRemoveTilesCmd,
				RemoveTileLayerCmd, AddTileLayerCmd, ChangeTileLayerNameCmd, MoveTileLayerCmd, AddGameObjectCmd,
				DeleteGameObjectCmd /* Add other command types */>;

class CommandManager : public AbstractCommandManager<CreateTileToolAddCmd, CreateTileToolRemoveCmd, RectToolAddTilesCmd,
													 RectToolRemoveTilesCmd, RemoveTileLayerCmd, AddTileLayerCmd,
													 ChangeTileLayerNameCmd, MoveTileLayerCmd, AddGameObjectCmd,
													 DeleteGameObjectCmd /* Add other command types */>
{
};

} // namespace SCION_EDITOR
