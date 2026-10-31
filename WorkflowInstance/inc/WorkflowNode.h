#ifndef WORKFLOW_NODE_H
#define WORKFLOW_NODE_H

#include <memory>
#include <string>

#include "NodeResult.h"
#include "NodeState.h"   // FIX 1: use Mandla's NodeState instead of a second, empty one

class WorkflowInstance;

class WorkflowNode {
private:
    std::string name;
    std::unique_ptr<WorkflowNode> next;
    std::unique_ptr<NodeState> state;

protected:
    virtual NodeResult process(WorkflowInstance& instance) = 0;

public:
    /// FIX 2: public, because the concrete states call node.setState(...)
    void setState(std::unique_ptr<NodeState> state);

    WorkflowNode(std::string name);
    virtual ~WorkflowNode() = default;

    void setNext(std::unique_ptr<WorkflowNode> next);
    std::string getName() const;
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
    virtual void cancel();
    virtual void reset();
    virtual void resetChain();

    virtual std::unique_ptr<WorkflowNode> clone() = 0;
    std::unique_ptr<WorkflowNode> cloneChain();
};

#endif
