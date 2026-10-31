/**
 * @file ResetWorkflowCommand.h
 * @brief Macro command: resets every node of a workflow instance, then the instance itself.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef RESETWORKFLOWCOMMAND_H
#define RESETWORKFLOWCOMMAND_H

#include <memory>
#include <vector>
#include "Command.h"

class WorkflowInstance;

/**
 * @class ResetWorkflowCommand
 * @brief Macro command: one ResetNodeCommand per node of the instance's chain, then instance->reset().
 *
 * @note Refused up front if the instance is cancelled, so no node is touched.
 *       Every other node state allows reset, so the reset cannot stop halfway.
 * @see ResetNodeCommand, WorkflowInstance::reset, AppealCommand
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class ResetWorkflowCommand : public Command {
public:
    /**
     * @brief Creates the command.
     * @param[in] instance The workflow instance to reset (not owned).
     */
    ResetWorkflowCommand(WorkflowInstance* instance);

    /**
     * @brief Resets every node, then the instance.
     * @pre The instance is not cancelled.
     * @post Every node is in Waiting; the instance's status is Created and its current node is the first node.
     * @retval true  The workflow was reset.
     * @retval false No instance, the instance is cancelled, or a node refused.
     */
    bool execute();

    /**
     * @brief Describes the command for the audit log.
     * @return e.g. "Reset workflow LoanRequest-0007".
     */
    std::string describe() const;

private:
    WorkflowInstance* instance;                      ///< the receiver (not owned)
    std::vector<std::unique_ptr<Command>> children;  ///< one ResetNodeCommand per node, built inside execute()
};

#endif
