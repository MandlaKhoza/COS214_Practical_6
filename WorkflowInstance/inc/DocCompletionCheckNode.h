#ifndef DOC_COMPLETION_CHECK_NODE_H
#define DOC_COMPLETION_CHECK_NODE_H

#include "WorkflowNode.h"
#include "NodeResult.h"
#include "WorkflowInstance.h"
#include <map>

class DocCompletionCheckNode : public WorkflowNode {
protected:
    NodeResult process(WorkflowInstance& instance) override;

public:
    explicit DocCompletionCheckNode(std::string name);
    std::unique_ptr<WorkflowNode> clone() override;
};

#endif
