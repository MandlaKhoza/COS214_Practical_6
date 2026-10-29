#ifndef RANGE_CHECK_NODE_H
#define RANGE_CHECK_NODE_H

#include "WorkflowNode.h"
#include <vector>
#include "Comparable.h"

class WorkflowInstance;
struct NodeResult;

class RangeCheckNode : public WorkflowNode {
private:
    std::vector<std::shared_ptr<Comparable>> rules;

protected:
    NodeResult process(WorkflowInstance& instance) override;

public:
    explicit RangeCheckNode(const std::string& name);
    std::unique_ptr<WorkflowNode> clone() const override;
};

#endif
