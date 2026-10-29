#ifndef DOC_COMPLETION_CHECK_NODE_H
#define DOC_COMPLETION_CHECK_NODE_H

#include "WorkflowNode.h"

class WorkflowInstance;
struct NodeResult;

class DocCompletionCheckNode : public WorkflowNode {
protected:
    NodeResult process(WorkflowInstance& instance) override;

public:
    explicit DocCompletionCheckNode(const std::string& name);
    std::unique_ptr<WorkflowNode> clone() const override;
};

#endif
