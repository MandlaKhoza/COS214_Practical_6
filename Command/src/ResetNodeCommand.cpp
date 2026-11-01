/**
 * @file ResetNodeCommand.cpp
 * @brief Implementation of ResetNodeCommand.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "ResetNodeCommand.h"

#include "WorkflowNode.h"

ResetNodeCommand::ResetNodeCommand(WorkflowNode* node) : node(node){}

bool ResetNodeCommand::execute() {
	if (!node) return false;

	node->reset();

	return true;
}

std::string ResetNodeCommand::describe() const{
	return "Reset node " + (node ? node->getName() : std::string("(none)"));
}
