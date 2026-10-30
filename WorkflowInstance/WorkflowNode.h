#ifndef WORKFLOW_NODE_H
#define WORKFLOW_NODE_H

#include <memory>
#include <string>

#include "NodeResult.h"

class WorkflowInstance;
class NodeState;

class WorkflowNode {
private:
    std::string name;
    std::unique_ptr<WorkflowNode> next;
    std::unique_ptr<NodeState> state;

protected:
    virtual NodeResult process(WorkflowInstance& instance) = 0;
    virtual bool validateInput(WorkflowInstance& instance, std::string reason) = 0;
    virtual void recordHistory(WorkflowInstance& instance, NodeResult result) = 0;
    void setState(std::unique_ptr<NodeState> state);

public:
    WorkflowNode(std::string name);
    virtual ~WorkflowNode() = default;

    void setNext(std::unique_ptr<WorkflowNode> next);
    std::string getName();
    WorkflowNode* getNext();
    HandleResult handle(WorkflowInstance& instance);
    NodeState* getState();
    std::string getStateName();

    virtual void start();
    virtual void pass();
    virtual void needsApproval();
    virtual void approve();
    virtual void reject();
    virtual void fail();
    virtual void retry();
    virtual void cancel();
    virtual void reset();
    virtual void resetChain();

    virtual std::unique_ptr<WorkflowNode> clone() = 0;
    std::unique_ptr<WorkflowNode> cloneChain();
};

#endif
