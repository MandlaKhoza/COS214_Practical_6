/**
 * @file test_reset_node.cpp
 * @brief GoogleTest unit tests for ResetNodeCommand (Command pattern), run against
 *        the real WorkflowNode and the real node states (State pattern).
 *
 * WorkflowNode is abstract, so the tests use FakeNode, a minimal concrete node.
 * A node is put into a state through the real transitions (start, reject, ...),
 * exactly as the engine would do it. Each test is Arrange / Act / Assert, and test
 * names start with the kind of case: Positive, Negative, Edge or Errors.
 *
 * @author Hafani Ramagoma
 * @date 2026-10-31
 */
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "NodeResult.h"
#include "ResetNodeCommand.h"
#include "WorkflowController.h"
#include "WorkflowNode.h"

namespace {

/// @brief Mock node: the smallest concrete WorkflowNode (its process() is never called here).
class FakeNode : public WorkflowNode {
public:
    explicit FakeNode(const std::string& name) : WorkflowNode(name) {}
    std::unique_ptr<WorkflowNode> clone() override {
        return std::unique_ptr<WorkflowNode>(new FakeNode(getName()));
    }
protected:
    NodeResult process(WorkflowInstance&) override { return NodeResult{PASSED, getName()}; }
};

}  // namespace

/// @brief Fixture: one node called "Credit check", which starts in Waiting.
class ResetNodeCommandTest : public ::testing::Test {
protected:
    FakeNode node{"Credit check"};
};

// ---------------------------------------------------------------- Positive

TEST_F(ResetNodeCommandTest, Positive_RejectedNodeGoesBackToWaiting) {
    // Arrange
    node.start();
    node.reject();
    ASSERT_EQ(node.getStateName(), "Rejected");
    ResetNodeCommand cmd(&node);
    // Act
    bool ok = cmd.execute();
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(node.getStateName(), "Waiting");
}

TEST_F(ResetNodeCommandTest, Positive_FailedNodeGoesBackToWaiting) {
    node.start();
    node.fail();
    ResetNodeCommand cmd(&node);

    EXPECT_TRUE(cmd.execute());
    EXPECT_EQ(node.getStateName(), "Waiting");
}

TEST_F(ResetNodeCommandTest, Positive_PassedNodeGoesBackToWaiting) {
    node.start();
    node.pass();
    ResetNodeCommand cmd(&node);

    EXPECT_TRUE(cmd.execute());
    EXPECT_EQ(node.getStateName(), "Waiting");
}

TEST_F(ResetNodeCommandTest, Positive_DescribeNamesTheNode) {
    ResetNodeCommand cmd(&node);
    EXPECT_EQ(cmd.describe(), "Reset node Credit check");
}

TEST_F(ResetNodeCommandTest, Positive_ResetNodeCanRunAgain) {
    // the point of a reset: a fresh start
    node.start();
    node.reject();
    ResetNodeCommand(&node).execute();

    EXPECT_NO_THROW(node.start());
    EXPECT_EQ(node.getStateName(), "InProgress");
}

// ---------------------------------------------------------------- Negative

TEST_F(ResetNodeCommandTest, Negative_CancelledNodeRefusesAndStaysCancelled) {
    node.cancel();
    ResetNodeCommand cmd(&node);

    EXPECT_THROW(cmd.execute(), std::logic_error);
    EXPECT_EQ(node.getStateName(), "Cancelled");
}

TEST_F(ResetNodeCommandTest, Negative_NodeStillInProgressRefuses) {
    // a node that is busy has nothing to reset yet
    node.start();
    ResetNodeCommand cmd(&node);

    EXPECT_THROW(cmd.execute(), std::logic_error);
    EXPECT_EQ(node.getStateName(), "InProgress");
}

TEST_F(ResetNodeCommandTest, Negative_RefusalIsLoggedByControllerWithReason) {
    // Arrange
    node.cancel();
    WorkflowController controller;
    // Act
    bool ok = controller.execute(std::unique_ptr<Command>(new ResetNodeCommand(&node)));
    // Assert
    EXPECT_FALSE(ok);
    ASSERT_EQ(controller.getAuditLog().size(), 1u);
    EXPECT_EQ(controller.getAuditLog()[0].description,
              "Reset node Credit check (refused: Cannot reset while Cancelled)");
}

// ---------------------------------------------------------------- Edge

TEST_F(ResetNodeCommandTest, Edge_WaitingNodeStaysWaitingAndSucceeds) {
    ResetNodeCommand cmd(&node);

    EXPECT_TRUE(cmd.execute());
    EXPECT_EQ(node.getStateName(), "Waiting");
}

TEST_F(ResetNodeCommandTest, Edge_PendingNodeGoesBackToWaiting) {
    // Agreed in the design: a node waiting for a human decision can be reset.
    node.start();
    node.needsApproval();
    ASSERT_EQ(node.getStateName(), "Pending");
    ResetNodeCommand cmd(&node);

    EXPECT_TRUE(cmd.execute());
    EXPECT_EQ(node.getStateName(), "Waiting");
}

TEST_F(ResetNodeCommandTest, Edge_OnlyTheGivenNodeIsReset) {
    // Arrange: two rejected nodes in a chain
    std::unique_ptr<WorkflowNode> second(new FakeNode("Fraud check"));
    WorkflowNode* secondPtr = second.get();
    node.setNext(std::move(second));
    node.start();      node.reject();
    secondPtr->start(); secondPtr->reject();
    // Act
    ResetNodeCommand(&node).execute();
    // Assert: resetting the whole chain is ResetWorkflowCommand's job, not this one's
    EXPECT_EQ(node.getStateName(), "Waiting");
    EXPECT_EQ(secondPtr->getStateName(), "Rejected");
}

TEST_F(ResetNodeCommandTest, Edge_ResettingTwiceIsHarmless) {
    node.start();
    node.reject();
    ResetNodeCommand cmd(&node);

    EXPECT_TRUE(cmd.execute());
    EXPECT_TRUE(cmd.execute());
    EXPECT_EQ(node.getStateName(), "Waiting");
}

// ---------------------------------------------------------------- Errors

TEST(ResetNodeCommandErrorsTest, Errors_NullNodeReturnsFalseWithoutCrashing) {
    ResetNodeCommand cmd(nullptr);

    bool ok = true;
    EXPECT_NO_THROW(ok = cmd.execute());
    EXPECT_FALSE(ok);
    EXPECT_EQ(cmd.describe(), "Reset node (none)");
}
