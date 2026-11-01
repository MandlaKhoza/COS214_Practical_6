/**
 * @file CancelWorkflowCommand.cpp
 * @brief Implementation of CancelWorkflowCommand.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "CancelWorkflowCommand.h"

#include "WorkflowInstance.h"

#include <string>
using namespace std;

CancelWorkflowCommand::CancelWorkflowCommand(WorkflowInstance* instance) : instance(instance) {}

bool CancelWorkflowCommand::execute() {
	if (!instance) return false;

	instance->cancel();

	return true;
}

string CancelWorkflowCommand::describe() const {
	return "Cancel workflow " + (instance ? instance->getId() : std::string("(none)"));
}
