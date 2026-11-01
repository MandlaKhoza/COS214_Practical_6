/**
 * @file WaitingState.cpp
 * @brief Implementation of WaitingState
 */
#include "WaitingState.h"
#include "InProgressState.h"
#include "CancelledState.h"
#include "WorkflowNode.h"
#include <memory>

/**
 * @brief Gives the name of this state.
 * @return "Waiting".
 * */
std::string WaitingState::getName() const { return "Waiting"; }

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Waiting to InProgress.
 * */
void WaitingState::start(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new InProgressState()));
}

/**
 * @param node the node being reset (unused, it is already Waiting)
 * @brief Waiting to Waiting. Allowed and changes nothing.
 * */
void WaitingState::reset(WorkflowNode&) {}

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Waiting to Cancelled.
 * */
void WaitingState::cancel(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new CancelledState()));
}