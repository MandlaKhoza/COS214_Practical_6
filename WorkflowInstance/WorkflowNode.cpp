#include "WorkflowNode.h"

#include <iostream>

void WorkflowNode::setState(std::unique_ptr<NodeState> state) {
    this->state = std::move(state);
}

WorkflowNode::WorkflowNode(std::string name){
    this->name = name;
}

void WorkflowNode::setNext(std::unique_ptr<WorkflowNode> next){
    this->next = std::move(next);
}

std::string WorkflowNode::getName() {
    return this->name;
}

WorkflowNode* WorkflowNode::getNext() {
    return this->next.get();
}

HandleResult WorkflowNode::handle(WorkflowInstance& instance) {
    // instance.setCurrent(this);
    // instance.markRunning();

    // NodeResult result = process(instance);
    // recordHistory(instance, result);

     // switch (res.result) {
        // case HandleResult::PASSED:
            // pass();
            // if (next) return next->handle(instance);
            // instance.complete();
            // return HandleResult::PASSED;
        // case HandleResult::REJECTED:
        //     reject();
            // instance.rejectNodeName(getName());
            // return HandleResult::REJECTED;
        // case HandleResult::FAILED:
        //     fail();
        //     return HandleResult::FAILED;
        // case HandleResult::PENDING:
            // needsApproval();   // stop; resume() will re-enter
            // return HandleResult::PENDING;
        // case HandleResult::CANCELLED:
        //     cancel();
            // instance.cancel();
            // return HandleResult::CANCELLED;
    // }     
  // }
    NodeResult nodeRes = process(instance);

    std::cout << nodeRes.name << " returned with result: " << nodeRes.result << std::endl;
    
    return nodeRes.result;
}

NodeState* WorkflowNode::getState() {
    return this->state.get();
}

std::string WorkflowNode::getStateName() {
    return this->state->getName();
}

void WorkflowNode::start()
{
    this->state->start(this);
}

void WorkflowNode::pass() {
    this->state->pass(this);
}

void WorkflowNode::needsApproval() {
    this->state->approve(this);
}

void WorkflowNode::approve() {
    this->state->approve(this);
}

void WorkflowNode::reject() {
    this->state->reject(this);
}

void WorkflowNode::fail() {
    this->state->fail(this);
}

void WorkflowNode::retry() {
    this->state->retry(this);
}

void WorkflowNode::cancel() {
    this->state->cancel(this);
}

void WorkflowNode::reset() {
    this->state->reset(this);
}

void WorkflowNode::resetChain() {
    // TODO
}

std::unique_ptr<WorkflowNode> WorkflowNode::cloneChain() {
    // TODO
}
