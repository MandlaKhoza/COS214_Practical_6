/**
 * @file CancelWorkflowCommand.h
 * @brief Command: cancels a workflow instance (the applicant withdraws).
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef CANCELWORKFLOWCOMMAND_H
#define CANCELWORKFLOWCOMMAND_H

#include "Command.h"

class WorkflowInstance;

/**
 * @class CancelWorkflowCommand
 * @brief ConcreteCommand: marks the instance and its current node as cancelled.
 *
 * The command only packages the request; WorkflowInstance::cancel() does the work
 * (sets the status, stops the current node, writes the history, notifies observers).
 *
 * @note A cancelled workflow can never be reset or appealed.
 * @see WorkflowInstance::cancel, WorkflowController
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class CancelWorkflowCommand : public Command{

private:
	WorkflowInstance* instance;  ///< the receiver (not owned)

public:
	/**
	 * @brief Creates the command.
	 * @param[in] instance The workflow instance to cancel (not owned).
	 */
	CancelWorkflowCommand(WorkflowInstance* instance);

	/**
	 * @brief Cancels the instance.
	 * @retval true  The instance was cancelled.
	 * @retval false No instance to act on.
	 * @throws std::logic_error If the workflow has already finished (passed up to the WorkflowController).
	 */
	bool execute();

	/**
	 * @brief Describes the command for the audit log.
	 * @return e.g. "Cancel workflow LoanRequest-0003".
	 */
	std::string describe() const;
};

#endif
