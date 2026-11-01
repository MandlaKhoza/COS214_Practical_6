/**
 * @file RejectedState.cpp
 * @brief Implementation of RejectedState.
 */
#include "RejectedState.h"
#include "WaitingState.h"
#include "WorkflowNode.h"
#include <memory>

/**
 * @brief Gives the name of this state.
 * @return "Rejected".
 * */
std::string RejectedState::getName() const { return "Rejected"; }

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Rejected to Waiting (gives the node a fresh start).
 * */
void RejectedState::reset(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new WaitingState()));
}