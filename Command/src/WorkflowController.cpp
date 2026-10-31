/**
 * @file WorkflowController.cpp
 * @brief Implementation of WorkflowController.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "WorkflowController.h"

WorkflowController::WorkflowController() {
	//does nothing as audit logger starts as an empty vector
}

bool WorkflowController::execute(std::unique_ptr<Command> cmd) {
	if(!cmd) return false;

	//get description for the log
	std::string description = cmd->describe();

	bool succeeded = false;
	try{
		succeeded = cmd->execute();
	}catch (const std::logic_error& e) {         // a state or the instance refused the action
        succeeded = false;
        description += " (refused: " + std::string(e.what()) + ")";
    }

	auditLog.push_back(CommandLogEntry{description, succeeded});

	return succeeded;
}

const std::vector<CommandLogEntry>& WorkflowController::getAuditLog() const {
	return auditLog;
}

/*
	how an audit log would look like:
	*Reset workflow LoanRequest-0007 | succeeded
	*Cancel workflow LoanRequest-0003 | succeeded
	*Reset workflow LoanRequest-0003 (refused: Cannot reset a cancelled workflow) | failed
*/
