#include "../inc/WorkflowNode.h"
#include "WaitingState.h"

#include <iostream>

#include <exception>
#include "WorkflowInstance.h"
#include "NodeResult.h"

void WorkflowNode::setState(std::unique_ptr<NodeState> state) {
    this->state = std::move(state);
}

WorkflowNode::WorkflowNode(std::string name) : name(name), state(new WaitingState()) {
    // FIX 3: every node starts in Waiting
}

void WorkflowNode::setNext(std::unique_ptr<WorkflowNode> next){
    this->next = std::move(next);
}

std::string WorkflowNode::getName() const {
    return this->name;
}

WorkflowNode* WorkflowNode::getNext() {
    return this->next.get();
}

HandleResult WorkflowNode::handle(WorkflowInstance& instance) {

    if(instance.isCancelled()){
        return HandleResult::CANCELLED;
    }

    instance.setCurrent(this);

    start();

    NodeResult nodeRes;
    try{
        nodeRes = process(instance);
    }
    catch (const std::exception& e){
        fail();
        instance.addHistory(getName(), e.what());

        return HandleResult::FAILED;
    }

    switch(nodeRes.result){

        case HandleResult::PASSED:
            pass();
            instance.addHistory(getName(), nodeRes.name);

            // Automatically execute the next node.
            if(next){
                return next->handle(instance);
            }
            instance.complete();
            return HandleResult::PASSED;


        case HandleResult::REJECTED:
            reject();
            instance.reject(getName());
            instance.addHistory(getName(), nodeRes.name);

            return HandleResult::REJECTED;


        case HandleResult::PENDING:
            needsApproval();
            instance.addHistory(getName(), nodeRes.name);
            return HandleResult::PENDING;

        case HandleResult::FAILED:
            fail();
            instance.addHistory(getName(), nodeRes.name);
            return HandleResult::FAILED;

        case HandleResult::CANCELLED:
            cancel();
            instance.cancel();
            instance.addHistory(getName(), nodeRes.name);
            return HandleResult::CANCELLED;
    }

    return HandleResult::FAILED;
}


NodeState* WorkflowNode::getState() {
    return this->state.get();
}

std::string WorkflowNode::getStateName() {
    return this->state->getName();
}

void WorkflowNode::start() {
    this->state->start(*this);
}

void WorkflowNode::pass() {
    this->state->pass(*this);
}

void WorkflowNode::needsApproval() {
    this->state->needsApproval(*this);
}

void WorkflowNode::approve() {
    this->state->approve(*this);
}

void WorkflowNode::reject() {
    this->state->reject(*this);
}

void WorkflowNode::fail() {
    this->state->fail(*this);
}

void WorkflowNode::cancel() {
    this->state->cancel(*this);
}

void WorkflowNode::reset() {
    this->state->reset(*this);
}

void WorkflowNode::resetChain() {
    reset();
    if (next) next->resetChain();
}

std::unique_ptr<WorkflowNode> WorkflowNode::cloneChain() {
    std::unique_ptr<WorkflowNode> copy = clone();
    if (next) copy->setNext(next->cloneChain());
    return copy;
}
