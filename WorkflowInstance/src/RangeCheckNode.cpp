#include "../inc/RangeCheckNode.h"
#include "../inc/NodeResult.h"
#include "ui_logger.hpp"

RangeCheckNode::RangeCheckNode(std::string name, std::vector<std::shared_ptr<Comparable>> rules) : WorkflowNode(name), rules(rules)
{}

NodeResult RangeCheckNode::process(WorkflowInstance& instance) {
    UI_LOG("[RANGE] Evaluating comparable field rules...");

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
                res.name = "Range rule failed for field '" + rule->getKey() + "'";
                UI_LOG("[RANGE] " + res.name);
                break;
            }
            UI_LOG("[RANGE] Field '" + rule->getKey() + "' is within range");
        } else {
            rangeSuccess = false;
            res.name = "Range rule missing field '" + rule->getKey() + "'";
            UI_LOG("[RANGE] " + res.name);
            break;
        }
    }

    if (rangeSuccess) {
        res.result = HandleResult::PASSED;
        res.name = "All range rules passed";
        UI_LOG("[RANGE] All range rules passed");
        return res;
    }
    res.result = HandleResult::REJECTED;
    return res;
}

std::unique_ptr<WorkflowNode> RangeCheckNode::clone() {
    std::string name = this->WorkflowNode::getName();

    return std::unique_ptr<WorkflowNode>(new RangeCheckNode(name, rules));
}
