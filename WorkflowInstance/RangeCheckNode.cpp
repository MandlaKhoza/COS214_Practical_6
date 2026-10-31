#include "RangeCheckNode.h"
#include "NodeResult.h"

RangeCheckNode::RangeCheckNode(std::string name, std::vector<std::shared_ptr<Comparable>> rules) : WorkflowNode(name), rules(rules)
{}

NodeResult RangeCheckNode::process(WorkflowInstance& instance) {
    auto document = instance.getDocument();
    std::map<std::string, double> comp = document->getComparable();

    bool rangeSuccess = true;
    NodeResult res;
    res.name = "RangeCheckNode";

    for (const auto& rule : rules) {
        auto it = comp.find(rule->getKey());

        if (it != comp.end())
        {
            if (!rule->compare(it->second)) { // If it is not within range or correct
                rangeSuccess = false;
                break;
            }
        } else {
            // Key does not exist
            rangeSuccess = false;
            break;
        }
    }

    if (rangeSuccess) {
        res.result = HandleResult::PASSED;
        return res;
    }
    // else
    res.result = HandleResult::FAILED;
    return res;
}

std::unique_ptr<WorkflowNode> RangeCheckNode::clone() {
    std::string name = this->WorkflowNode::getName();

    return std::unique_ptr<WorkflowNode>(new RangeCheckNode(name, rules));
}
