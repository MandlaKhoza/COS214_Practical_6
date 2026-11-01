/**
 * @file NodeState.cpp
 * @brief Default behaviour of the abstract State (NodeState).
 */
#include "NodeState.h"
#include "WorkflowNode.h"
#include "CancelledState.h"
#include <memory>
#include <stdexcept>

/**
 * @param action the name of the refused event ("start")
 * @brief Throws std::logic_error("Cannot <action> while <name>").
 * @throws std::logic_error always.
 * */
void NodeState::invalid(const std::string& action) const {
    throw std::logic_error("Cannot " + action + " while " + getName());
}

/**
 * @param node the node the event was sent to (unused, the event is refused)
 * @brief Default: refused. Only WaitingState allows start.
 * */
void NodeState::start(WorkflowNode&) { invalid("start"); }

/**
 * @param node the node the event was sent to (unused, the event is refused)
 * @brief Default: refused. Only InProgressState allows pass.
 * */
void NodeState::pass(WorkflowNode&) { invalid("pass"); }

/**
 * @param node the node the event was sent to (unused, the event is refused)
 * @brief Default: refused. Only InProgressState allows needsApproval.
 * */
void NodeState::needsApproval(WorkflowNode&) { invalid("needsApproval"); }

/**
 * @param node the node the event was sent to (unused, the event is refused)
 * @brief Default: refused. Only PendingState allows approve.
 * */
void NodeState::approve(WorkflowNode&) { invalid("approve"); }

/**
 * @param node the node the event was sent to (unused, the event is refused)
 * @brief Default: refused. InProgressState and PendingState allow reject.
 * */
void NodeState::reject(WorkflowNode&) { invalid("reject"); }

/**
 * @param node the node the event was sent to (unused, the event is refused)
 * @brief Default: refused. Only InProgressState allows fail.
 * */
void NodeState::fail(WorkflowNode&) { invalid("fail"); }

/**
 * @param node the node the event was sent to (unused, the event is refused)
 * @brief Default: refused. Waiting, Rejected, Failed and Passed override reset.
 * */
void NodeState::reset(WorkflowNode&) { invalid("reset"); }

/**
 * @param node this would be the current node to where the state must be changed
 * @brief Default cancel: any non-final state toCancelled; final states refuse.
 * @throws std::logic_error if the state is finalt
 * @note setState() is the last statement because it destroys this object.
 * */
void NodeState::cancel(WorkflowNode& node) {
    if (isFinal()) {
        invalid("cancel");
    }
    node.setState(std::unique_ptr<NodeState>(new CancelledState()));
}