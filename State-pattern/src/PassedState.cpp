/**
 * @file PassedState.cpp
 * @brief Implementation of PassedState.
 */
#include "PassedState.h"
#include "WaitingState.h"
#include "WorkflowNode.h"
#include <memory>

/**
 * @brief Gives the name of this state.
 * @return "Passed".
 * */
std::string PassedState::getName() const { return "Passed"; }

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Passed to Waiting. Only used by a whole-chain reset.
 * */
void PassedState::reset(WorkflowNode& node) {
    node.setState(std::unique_ptr<NodeState>(new WaitingState()));
}