#ifndef WORKFLOW_DEFINITION_H
#define WORKFLOW_DEFINITION_H

#include <memory>
#include <string>

class WorkflowNode;

class WorkflowDefinition {
private:
    std::string workflowType;
    std::unique_ptr<WorkflowNode> head;

public:
    explicit WorkflowDefinition(const std::string& workflowType);
    ~WorkflowDefinition();

    void setHead(std::unique_ptr<WorkflowNode> node);
    WorkflowNode* getHead() const;
    std::unique_ptr<WorkflowNode> createChain() const;
};

#endif
