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
#include <stdexcept>
#include <string>


ResetWorkflowCommand::ResetWorkflowCommand(WorkflowInstance* instance) : instance(instance){}

bool ResetWorkflowCommand::execute() {
	if (!instance) return false;

	//if whole workflow is cancelled then it cant be reset or brought back to life
	if (instance->isCancelled())
		return false;

	//pass 1: check that every node can be reset BEFORE changing any of them,
	//so a refusal leaves the whole workflow exactly as it was (all-or-nothing)
	for (WorkflowNode* node = instance->getChain(); node != nullptr; node = node->getNext()){
		std::string state = node->getStateName();
		if (state == "Cancelled" || state == "InProgress")
			throw std::logic_error("Cannot reset: " + node->getName() + " is " + state);
	}

	//pass 2: safe now, so build one ResetNodeCommand per node (the macro command)
	children.clear();
	for (WorkflowNode* node = instance->getChain(); node != nullptr; node = node->getNext()){
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
