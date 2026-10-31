/**
 * @file Command.h
 * @brief Command interface of the Command pattern: one user action on a workflow.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef COMMAND_H
#define COMMAND_H

#include <string>

/**
 * @class Command
 * @brief Abstract Command: one user action on a workflow, packaged as an object.
 *
 * The Façade never acts on a workflow directly. It creates a Command and hands it
 * to the WorkflowController (the Invoker), which runs it and logs it. Because each
 * request is an object, it can be logged, refused, or combined with others
 * (ResetWorkflowCommand is made of ResetNodeCommands).
 *
 * ### Example Usage:
 * @code
 * controller.execute(std::unique_ptr<Command>(new CancelWorkflowCommand(instance)));
 * @endcode
 *
 * @see WorkflowController, CancelWorkflowCommand, ResetNodeCommand,
 *      ResetWorkflowCommand
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class Command {
	public:
		/// @brief Virtual destructor so commands are destroyed correctly through a Command pointer.
		virtual ~Command() {}

		/**
		 * @brief Carries out the action on the receiver.
		 * @retval true  The action was carried out.
		 * @retval false The action was refused (e.g. nothing to act on).
		 * @throws std::logic_error If the receiver refuses a state change; the
		 *         WorkflowController catches it and logs the command as failed.
		 */
		virtual bool execute() = 0;

		/**
		 * @brief Describes the action for the audit log.
		 * @return e.g. "Reset workflow LoanRequest-0007".
		 */
		virtual std::string describe() const = 0;
};

#endif
