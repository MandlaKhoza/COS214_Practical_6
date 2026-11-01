/**
 * @file WorkflowController.h
 * @brief Invoker of the Command pattern: runs every command and logs it.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef WORKFLOWCONTROLLER_H
#define WORKFLOWCONTROLLER_H

#include <memory>
#include <vector>
#include "Command.h"
#include "CommandLogEntry.h"

/**
 * @class WorkflowController
 * @brief Invoker: runs commands and logs every one, succeeded or not.
 *
 * Every user action on a workflow (cancel, reset, appeal) goes through here, so the
 * audit log is complete. A refusal from a state (std::logic_error) becomes a failed
 * log entry and a false return value instead of a crash.
 *
 * ### Example Usage:
 * @code
 * WorkflowController controller;
 * bool ok = controller.execute(std::unique_ptr<Command>(new ResetWorkflowCommand(instance)));
 * // audit log line: "Reset workflow LoanRequest-0007 | succeeded"
 * @endcode
 *
 * @see Command, CommandLogEntry, TividyFacade
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class WorkflowController {
public:
    /// @brief Creates a controller with an empty audit log.
    WorkflowController();

    /**
     * @brief Runs a command and logs the attempt.
     *
     * Takes ownership of the command; it is deleted when this function returns.
     *
     * @param[in] cmd The command to run.
     * @retval true  The command succeeded.
     * @retval false No command was given (not logged), it returned false, or it was
     *               refused with std::logic_error (logged with the reason).
     */
    bool execute(std::unique_ptr<Command> cmd);

    /**
     * @brief Returns every logged command, oldest first.
     * @return The audit log (read-only).
     */
    const std::vector<CommandLogEntry>& getAuditLog() const;

private:
    std::vector<CommandLogEntry> auditLog;  ///< one entry per executed command
};

#endif
