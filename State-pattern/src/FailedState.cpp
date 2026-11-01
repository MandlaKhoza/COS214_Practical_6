/**
 * @file FailedState.cpp
 * @brief Implementation of FailedState.
 */
#include "FailedState.h"
#include "WaitingState.h"
#include "CancelledState.h"
#include "WorkflowNode.h"
#include <memory>

/**
 * @brief Gives the name of this state.
 * @return "Failed".
 * */
std::string FailedState::getName() const { return "Failed"; }

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Failed ro Waiting, so the node can be run again.
 * */
void FailedState::retry(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new WaitingState()));
}

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Failed tp Waiting.
 * */
void FailedState::reset(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new WaitingState()));
}

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Failed tp Cancelled.
 * */
void FailedState::cancel(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new CancelledState()));
}
