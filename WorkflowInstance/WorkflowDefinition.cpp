#include "WorkflowDefinition.h"
#include "WorkflowNode.h"

WorkflowDefinition::WorkflowDefinition(const std::string& workflowType)
    : workflowType(workflowType), head(nullptr) {}

WorkflowDefinition::~WorkflowDefinition() = default;

void WorkflowDefinition::setHead(std::unique_ptr<WorkflowNode> node) {
    head = std::move(node);
}

WorkflowNode* WorkflowDefinition::getHead() const {
    return head.get();
}

std::unique_ptr<WorkflowNode> WorkflowDefinition::createChain() const {
    // Implement the logic to create a chain of WorkflowNodes
    return nullptr; // Replace with actual implementation
}
