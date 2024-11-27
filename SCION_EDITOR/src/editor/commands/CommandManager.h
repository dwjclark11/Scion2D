#pragma once
#include "AbstractCommandManager.h"
#include "cmds/CreateTileToolCmds.h"
// TODO: add other commands as necessary

namespace SCION_EDITOR
{
using UndoableCommands = VarCommands<CreateTileToolAddCmd /* Add other command types */>;

class CommandManager : public AbstractCommandManager<CreateTileToolAddCmd /* Add other command types */>
{
};

} // namespace SCION_EDITOR
