/**
 * @file ResetNodeCommand.h
 * @brief Command: puts one workflow node back to Waiting.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef RESETNODECOMMAND_H
#define RESETNODECOMMAND_H
#include "Command.h"

class WorkflowNode;

/**
 * @class ResetNodeCommand
 * @brief ConcreteCommand: puts one node back to Waiting.
 *
 * Whether the reset is allowed is decided by the node's current state (State
 * pattern): every state allows it except Cancelled, which throws.
 *
 * @note Used on its own, and as a child of the macro command ResetWorkflowCommand.
 * @see WorkflowNode::reset, ResetWorkflowCommand
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class ResetNodeCommand : public Command{

private:
	WorkflowNode* node;  ///< the receiver (not owned)

public:
	/**
	 * @brief Creates the command.
	 * @param[in] node The node to reset (not owned).
	 */
	ResetNodeCommand(WorkflowNode* node);

	/**
	 * @brief Resets the node.
	 * @retval true  The node is back in Waiting.
	 * @retval false No node to act on.
	 * @throws std::logic_error If the node is Cancelled (thrown by its state).
	 */
	bool execute();

	/**
	 * @brief Describes the command for the audit log.
	 * @return e.g. "Reset node Credit check".
	 */
	std::string describe() const;
};

#endif
