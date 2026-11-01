#include "../inc/DocCompletionCheckNode.h"
#include "ui_logger.hpp"
#include <cmath>
DocCompletionCheckNode::DocCompletionCheckNode(std::string name) : WorkflowNode(name){}

NodeResult DocCompletionCheckNode::process(WorkflowInstance& wi) {
    UI_LOG("[DOC] Checking document completeness...");

    const auto& commonFields     = wi.getDocument()->getCommon();
    const auto& comparableFields = wi.getDocument()->getComparable();

    for (const auto& field : commonFields) {
        if (field.second.empty()) {
            const std::string msg = "Document not completed: empty field '" + field.first + "'";
            UI_LOG("[DOC] " + msg);
            return NodeResult{
                HandleResult::REJECTED,
                msg
            };
        }
    }

    for (const auto& field : comparableFields) {
        if (std::isnan(field.second)) {
            const std::string msg = "Document not completed: missing numeric field '" + field.first + "'";
            UI_LOG("[DOC] " + msg);
            return NodeResult{
                HandleResult::REJECTED,
                msg
            };
        }
    }

    UI_LOG("[DOC] Document is properly filled");
    return NodeResult{ HandleResult::PASSED, "Document is properly filled" };
}

std::unique_ptr<WorkflowNode> DocCompletionCheckNode::clone(){
    std::string name = this->WorkflowNode::getName();

    return std::unique_ptr<WorkflowNode>(new DocCompletionCheckNode(name));
}
