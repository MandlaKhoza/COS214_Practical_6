#ifndef APPROVAL_NODE_H
#define APPROVAL_NODE_H

#include "WorkflowNode.h"
#include <memory>

class ApprovalChain;
class WorkflowInstance;
struct NodeResult;

class ApprovalNode : public WorkflowNode {
private:
    std::shared_ptr<ApprovalChain> approvals;

protected:
    NodeResult process(WorkflowInstance& instance) override;

public:
    ApprovalNode(std::string name,
                 std::shared_ptr<ApprovalChain> approvals);
    std::unique_ptr<WorkflowNode> clone() override;
};

#endif
