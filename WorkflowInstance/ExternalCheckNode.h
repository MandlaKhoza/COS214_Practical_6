#ifndef EXTERNAL_CHECK_NODE_H
#define EXTERNAL_CHECK_NODE_H

#include "WorkflowNode.h"
#include <memory>

class ExternalCheck;
class WorkflowInstance;
struct NodeResult;

class ExternalCheckNode : public WorkflowNode {
private:
    std::shared_ptr<ExternalCheck> check;

protected:
    NodeResult process(WorkflowInstance& instance) override;

public:
    ExternalCheckNode(std::string name,
                      std::shared_ptr<ExternalCheck> check);
    std::unique_ptr<WorkflowNode> clone() override;
};

#endif
