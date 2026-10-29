#ifndef WORKFLOW_NODE_H
#define WORKFLOW_NODE_H

#include <memory>
#include <string>

class WorkflowInstance;
class NodeState;
struct HandleResult;
struct NodeResult;

class WorkflowNode {
private:
    std::string name;
    std::unique_ptr<WorkflowNode> next;
    std::unique_ptr<NodeState> state;

protected:
    virtual NodeResult process(WorkflowInstance& instance) = 0;
    virtual bool validateInput(const WorkflowInstance& instance,
                               const std::string& reason) = 0;
    virtual void recordHistory(WorkflowInstance& instance,
                              const NodeResult& result) = 0;
    void setState(std::unique_ptr<NodeState> state);

public:
    explicit WorkflowNode(const std::string& name);
    virtual ~WorkflowNode();

    WorkflowNode(const WorkflowNode&) = delete;
    WorkflowNode& operator=(const WorkflowNode&) = delete;

    void setNext(std::unique_ptr<WorkflowNode> next);
    const std::string& getName() const;
    WorkflowNode* getNext() const;
    HandleResult handle(WorkflowInstance& instance);
    NodeState* getState() const;
    const std::string& getStateName() const;

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

    virtual std::unique_ptr<WorkflowNode> clone() const = 0;
    std::unique_ptr<WorkflowNode> cloneChain() const;
};

#endif
