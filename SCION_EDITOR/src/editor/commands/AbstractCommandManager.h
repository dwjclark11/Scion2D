#pragma once

#include <variant>
#include <stack>

namespace SCION_EDITOR
{
/*
 * @brief Concept for Command Interface to ensure that all structs/types passed
 * into the command manager have the necessary undo/redo functionality.
 */
template <typename T>
concept CommandType = requires( T t ) {
	{
		t.undo()
	} -> std::same_as<void>;
	{
		t.redo()
	} -> std::same_as<void>;
};

template <CommandType... Cmds>
using VarCommands = std::variant<Cmds...>;

template <CommandType... Cmds>
using CommandStack = std::stack<VarCommands<Cmds...>>;

template <CommandType... Cmds>
class AbstractCommandManager
{
  public:
	/*
	 * @brief Runs Undo function for whatever action/cmd is on the undo stack.
	 * Pushes that function onto the redo stack.
	 */
	void Undo();

	/*
	 * @brief Runs Redo function for whatever action/cmd is on the redo stack.
	 * Pushes that function onto the undo stack.
	 */
	void Redo();

	/*
	 * @brief Clears both the undo and redo stacks.
	 */
	void Clear();

	/*
	 * @brief Checks to see if there are any commands to undo.
	 * @return Returns true if empty, false otherwise.
	 */
	bool UndoEmpty() const noexcept { return m_UndoStack.empty(); }

	/*
	 * @brief Checks to see if there are any commands to redo.
	 * @return Returns true if empty, false otherwise.
	 */
	bool RedoEmpty() const noexcept { return m_RedoStack.empty(); }

	/*
	 * @brief Clears the redo stack and pushes the passed in command onto the
	 * Undo stack.
	 * @param Takes in a specified std::variant VarCommands.
	 */
	void Execute( VarCommands<Cmds...>& undoableVariant );

  private:
	void RunUndo( VarCommands<Cmds...>& undoVariant );

	template <CommandType TCmd>
	void RunUndo_Impl( TCmd& undoCmd );

	void RunRedo( VarCommands<Cmds...>& undoVariant );

	template <CommandType TCmd>
	void RunRedo_Impl( TCmd& undoCmd );

  private:
	CommandStack<Cmds...> m_UndoStack;
	CommandStack<Cmds...> m_RedoStack;
};

} // namespace SCION_EDITOR

#include "editor/commands/AbstractCommandManager.inl"
