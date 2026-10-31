#include "DocCompletionCheckNode.h"
#include <cmath>
DocCompletionCheckNode::DocCompletionCheckNode(std::string name) : WorkflowNode(name){}

NodeResult DocCompletionCheckNode::process(WorkflowInstance& wi) {
    const auto& commonFields     = wi.getDocument()->getCommon();
    const auto& comparableFields = wi.getDocument()->getComparable();

    for (const auto& field : commonFields) {
        if (field.second.empty()) {
            return NodeResult{
                HandleResult::FAILED,
                "Document not completed: empty field '" + field.first + "'"
            };
        }
    }

    for (const auto& field : comparableFields) {
        if (std::isnan(field.second)) {
            return NodeResult{
                HandleResult::FAILED,
                "Document not completed: missing numeric field '" + field.first + "'"
            };
        }
    }

    return NodeResult{ HandleResult::PASSED, "Document is properly filled" };
}

std::unique_ptr<WorkflowNode> DocCompletionCheckNode::clone(){
    std::string name = this->WorkflowNode::getName();

    return std::unique_ptr<WorkflowNode>(new DocCompletionCheckNode(name));
}
