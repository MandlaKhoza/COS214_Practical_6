#include "DocCompletionCheckNode.h"
#include <cmath>
DocCompletionCheckNode::DocCompletionCheckNode(std::string name) : WorkflowNode(name){}

NodeResult DocCompletionCheckNode::process(WorkflowInstance& wi) {
    const auto& commonFields     = wi.getDocument()->getCommon();
    const auto& comparableFields = wi.getDocument()->getComparable();

    for (const auto& [key, value] : commonFields) {
        if (value.empty()) {
            return NodeResult{
                HandleResult::FAILED,
                "Document not completed: empty field '" + key + "'"
            };
        }
    }

    for (const auto& [key, value] : comparableFields) {
        if (std::isnan(value)) {
            return NodeResult{
                HandleResult::FAILED,
                "Document not completed: missing numeric field '" + key + "'"
            };
        }
    }

    return NodeResult{ HandleResult::PASSED, "Document is properly filled" };
}

std::unique_ptr<WorkflowNode> DocCompletionCheckNode::clone(){
    std::string name = this->WorkflowNode::getName();

    return std::unique_ptr<WorkflowNode>(new DocCompletionCheckNode(name));
}
