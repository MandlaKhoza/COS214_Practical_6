/**
 * @file ResetWorkflowCommand.cpp
 * @brief Implementation of the ResetWorkflowCommand macro command.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "ResetWorkflowCommand.h"
#include "ResetNodeCommand.h"
#include "WorkflowNode.h"
#include "WorkflowInstance.h"


ResetWorkflowCommand::ResetWorkflowCommand(WorkflowInstance* instance) : instance(instance){}

bool ResetWorkflowCommand::execute() {
	if (!instance) return false;

	//if whole workflow is cancelled then it cant be reset or brought back to life
	if (instance->isCancelled())
		return false;

	//enabled the macro command where we do a ResetNodeCommand per node in this particular instance.
	children.clear();
	for (WorkflowNode* node = instance->getChain(); node != nullptr; node = node->getNext()){
		//building the workflow again, node by node
		children.push_back(std::unique_ptr<Command>(new ResetNodeCommand(node)));
	}

	//start the workflow again, event by event
	for(size_t i=0; i<children.size(); ++i){
		if (!children[i]->execute()) return false;      // a node refused → whole reset fails
	}

	instance->reset();
	
	return true;

}

std::string ResetWorkflowCommand::describe() const {
	 return "Reset workflow " + (instance ? instance->getId() : std::string("(none)"));
}
