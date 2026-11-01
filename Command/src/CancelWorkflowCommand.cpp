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

bool CancelWorkflowCommand::execute(){
    if (!instance) return false;

    if(instance->isFinished() || instance->isCancelled())
        return false;

    WorkflowNode* current = instance->getCurrent();

    if(current){
        current->cancel();
    }

    instance->cancel();

    return true;
}


string CancelWorkflowCommand::describe() const {
	return "Cancel workflow " + (instance ? instance->getId() : std::string("(none)"));
}
