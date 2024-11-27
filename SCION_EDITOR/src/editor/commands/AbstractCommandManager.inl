#include "AbstractCommandManager.h"

namespace SCION_EDITOR
{
template <CommandType... Cmds>
inline void AbstractCommandManager<Cmds...>::Undo()
{
	if ( m_UndoStack.empty() )
		return;

	RunUndo( m_UndoStack.top() );
	m_RedoStack.push( m_UndoStack.top() );
	m_UndoStack.pop();
}

template <CommandType... Cmds>
inline void AbstractCommandManager<Cmds...>::Redo()
{
	if ( m_RedoStack.empty() )
		return;

	RunRedo( m_RedoStack.top() );
	m_UndoStack.push( m_RedoStack.top() );
	m_RedoStack.pop();
}

template <CommandType... Cmds>
inline void AbstractCommandManager<Cmds...>::Clear()
{
	m_UndoStack = {};
	m_RedoStack = {};
}

template <CommandType... Cmds>
inline void AbstractCommandManager<Cmds...>::Execute( VarCommands<Cmds...>& undoableVariant )
{
	m_RedoStack = {};
	m_UndoStack.push( std::move( undoableVariant ) );
}

template <CommandType... Cmds>
inline void AbstractCommandManager<Cmds...>::RunUndo( VarCommands<Cmds...>& undoVariant )
{
	std::visit( [ & ]( auto&& val ) { RunUndo_Impl( val ); }, undoVariant );
}

template <CommandType... Cmds>
inline void AbstractCommandManager<Cmds...>::RunRedo( VarCommands<Cmds...>& undoVariant )
{
	std::visit( [ & ]( auto&& val ) { RunRedo_Impl( val ); }, undoVariant );
}

template <CommandType... Cmds>
template <CommandType TCmd>
inline void AbstractCommandManager<Cmds...>::RunUndo_Impl( TCmd& undoCmd )
{
	undoCmd.undo();
}

template <CommandType... Cmds>
template <CommandType TCmd>
inline void AbstractCommandManager<Cmds...>::RunRedo_Impl( TCmd& undoCmd )
{
	undoCmd.redo();
}

}
