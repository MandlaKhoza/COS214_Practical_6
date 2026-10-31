/**
 * @file InProgressState.cpp
 * @brief Implementation of InProgressState.
 * @note cancel() is inherited from NodeState: InProgress is not final to Cancelled.
 */
#include "InProgressState.h"
#include "PassedState.h"
#include "PendingState.h"
#include "RejectedState.h"
#include "FailedState.h"
#include "WorkflowNode.h"
#include <memory>

/**
 * @brief Gives the name of this state.
 * @return "InProgress".
 * */
std::string InProgressState::getName() const { return "InProgress"; }

/**
 * @param node this would be the current node to where the state must be changed
 * @brief InProgress to Passed (check succeeded).
 * */
void InProgressState::pass(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new PassedState()));
}

/**
 * @param node this would be the current node to where the state must be changed
 * @brief InProgress to Pending (a human decision is needed).
 * */
void InProgressState::needsApproval(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new PendingState()));
}

/**
 * @param node this would be the current node to where the state must be changed
 * @brief InProgress to Rejected (check failed).
 * */
void InProgressState::reject(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new RejectedState()));
}

/**
 * @param node this would be the current node to where the state must be changed
 * @brief InProgress to Failed (technical error).
 * */
void InProgressState::fail(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new FailedState()));
}