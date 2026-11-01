/**
 * @file FakeNode.h
 * @brief Test helper: the smallest concrete WorkflowNode, used to build chains in tests.
 * @author Hafani Ramagoma
 * @date 2026-10-31
 */
#ifndef FAKENODE_H
#define FAKENODE_H

#include <memory>
#include <string>

#include "NodeResult.h"
#include "WorkflowNode.h"

/// @brief Mock node: has a name and a state; its process() is never called by these tests.
class FakeNode : public WorkflowNode {
public:
    explicit FakeNode(const std::string& name) : WorkflowNode(name) {}
    std::unique_ptr<WorkflowNode> clone() override {
        return std::unique_ptr<WorkflowNode>(new FakeNode(getName()));
    }
protected:
    NodeResult process(WorkflowInstance&) override { return NodeResult{PASSED, getName()}; }
};

#endif
