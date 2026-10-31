/**
 * @file PendingState.cpp
 * @brief Implementation of PendingState.
 * @note PendingState.h was not among the uploaded files; it is supplied
 *       because InProgressState::needsApproval() moves a node here.
 *       cancel() is inherited from NodeState: Pending is not final to Cancelled.
 */
#include "PendingState.h"
#include "PassedState.h"
#include "RejectedState.h"
#include "WorkflowNode.h"
#include <memory>

/**
 * @brief Gives the name of this state.
 * @return "Pending".
 * */
std::string PendingState::getName() const { return "Pending"; }

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Pending to Passed (approved by a human).
 * */
void PendingState::approve(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new PassedState()));
}

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Pending tp Rejected (declined by a human).
 * */
void PendingState::reject(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new RejectedState()));
}