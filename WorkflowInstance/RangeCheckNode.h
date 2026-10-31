#ifndef RANGE_CHECK_NODE_H
#define RANGE_CHECK_NODE_H

#include "WorkflowNode.h"
#include "WorkflowInstance.h"
#include "Document.h"
#include "Comparable.h"

#include <vector>
#include <map>
#include <iostream>
#include <string>

class WorkflowInstance;

class RangeCheckNode : public WorkflowNode {
private:
    std::vector<std::shared_ptr<Comparable>> rules;

protected:
    NodeResult process(WorkflowInstance& instance) override;

public:
    RangeCheckNode(std::string name, std::vector<std::shared_ptr<Comparable>> rules);
    std::unique_ptr<WorkflowNode> clone() override;
};

#endif
