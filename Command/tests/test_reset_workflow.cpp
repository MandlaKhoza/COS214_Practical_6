/**
 * @file test_reset_workflow.cpp
 * @brief GoogleTest unit tests for ResetWorkflowCommand, the macro command of the
 *        Command pattern, run against the real WorkflowInstance, WorkflowNode and
 *        node states.
 *
 * ResetWorkflowCommand builds one ResetNodeCommand per node in the chain, runs them
 * all, then resets the instance itself. Each test is Arrange / Act / Assert, and test
 * names start with the kind of case: Positive, Negative, Edge or Errors.
 *
 * @author Hafani Ramagoma
 * @date 2026-10-31
 */
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

#include "ResetWorkflowCommand.h"
#include "WorkflowController.h"
#include "WorkflowInstance.h"
#include "support/FakeNode.h"

/**
 * @brief Fixture: a loan workflow with a three-node chain
 *        Identity check → Credit check → Officer approval.
 */
class ResetWorkflowCommandTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::unique_ptr<WorkflowNode> n1(new FakeNode("Identity check"));
        std::unique_ptr<WorkflowNode> n2(new FakeNode("Credit check"));
        std::unique_ptr<WorkflowNode> n3(new FakeNode("Officer approval"));
        first = n1.get();
        second = n2.get();
        third = n3.get();
        n2->setNext(std::move(n3));
        n1->setNext(std::move(n2));
        instance.setChain(std::move(n1));
        instance.markRunning();
    }

    WorkflowInstance instance{"LoanRequest-0007", "Loan", nullptr};
    WorkflowNode* first = nullptr;
    WorkflowNode* second = nullptr;
    WorkflowNode* third = nullptr;
};

TEST_F(ResetWorkflowCommandTest, Negative_RefusedResetChangesNothing) {
    // Arrange: node 2 cannot be reset, nodes 1 and 3 could be
    first->start();  first->reject();
    second->cancel();
    third->start();  third->pass();
    instance.complete();
    // Act
    EXPECT_THROW(ResetWorkflowCommand(&instance).execute(), std::logic_error);
    // Assert: all-or-nothing, so nothing was reset, not even node 1
    EXPECT_EQ(first->getStateName(), "Rejected");
    EXPECT_EQ(second->getStateName(), "Cancelled");
    EXPECT_EQ(third->getStateName(), "Passed");
    EXPECT_TRUE(instance.isFinished());
}

// ---------------------------------------------------------------- Positive

TEST_F(ResetWorkflowCommandTest, Positive_EveryNodeGoesBackToWaiting) {
    // Arrange: first passed, second rejected, third never reached
    first->start();  first->pass();
    second->start(); second->reject();
    ResetWorkflowCommand cmd(&instance);
    // Act
    bool ok = cmd.execute();
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_EQ(first->getStateName(), "Waiting");
    EXPECT_EQ(second->getStateName(), "Waiting");
    EXPECT_EQ(third->getStateName(), "Waiting");
}

TEST_F(ResetWorkflowCommandTest, Positive_FinishedWorkflowIsNoLongerFinished) {
    first->start();  first->pass();
    second->start(); second->pass();
    third->start();  third->pass();
    instance.complete();
    ASSERT_TRUE(instance.isFinished());

    EXPECT_TRUE(ResetWorkflowCommand(&instance).execute());
    EXPECT_FALSE(instance.isFinished());
}

TEST_F(ResetWorkflowCommandTest, Positive_CurrentNodeIsBackAtTheStart) {
    instance.setCurrent(third);

    ResetWorkflowCommand(&instance).execute();

    EXPECT_EQ(instance.getCurrent(), first);
}

TEST_F(ResetWorkflowCommandTest, Positive_ResetIsRecordedInHistory) {
    ResetWorkflowCommand(&instance).execute();

    ASSERT_FALSE(instance.getHistory().empty());
    EXPECT_EQ(instance.getHistory().back().message, "Reset: all steps back to Waiting, ready to run again");
}

TEST_F(ResetWorkflowCommandTest, Positive_DescribeNamesTheWorkflow) {
    EXPECT_EQ(ResetWorkflowCommand(&instance).describe(), "Reset workflow LoanRequest-0007");
}

TEST_F(ResetWorkflowCommandTest, Positive_PendingApprovalIsWithdrawn) {
    // the officer had not decided yet when the workflow was reset
    first->start();  first->pass();
    second->start(); second->pass();
    third->start();  third->needsApproval();

    EXPECT_TRUE(ResetWorkflowCommand(&instance).execute());
    EXPECT_EQ(third->getStateName(), "Waiting");
}

// ---------------------------------------------------------------- Negative

TEST_F(ResetWorkflowCommandTest, Negative_CancelledWorkflowIsRefusedAndNodesUntouched) {
    second->start(); second->reject();
    instance.cancel();

    EXPECT_FALSE(ResetWorkflowCommand(&instance).execute());
    EXPECT_EQ(second->getStateName(), "Rejected");
    EXPECT_TRUE(instance.isCancelled());
}

TEST_F(ResetWorkflowCommandTest, Negative_CancelledNodeMakesTheResetFail) {
    // Arrange
    second->cancel();
    WorkflowController controller;
    // Act
    bool ok = controller.execute(std::unique_ptr<Command>(new ResetWorkflowCommand(&instance)));
    // Assert: the node's refusal reaches the log as the reason
    EXPECT_FALSE(ok);
    ASSERT_EQ(controller.getAuditLog().size(), 1u);
    EXPECT_EQ(controller.getAuditLog()[0].description,
              "Reset workflow LoanRequest-0007 (refused: Cannot reset: Credit check is Cancelled)");
}

TEST_F(ResetWorkflowCommandTest, Negative_NodeStillRunningMakesTheResetFail) {
    first->start();  // InProgress: busy, not resettable

    EXPECT_THROW(ResetWorkflowCommand(&instance).execute(), std::logic_error);
}

// ---------------------------------------------------------------- Edge

TEST_F(ResetWorkflowCommandTest, Edge_ResetOfAFreshWorkflowChangesNothingAndSucceeds) {
    EXPECT_TRUE(ResetWorkflowCommand(&instance).execute());
    EXPECT_EQ(first->getStateName(), "Waiting");
    EXPECT_EQ(third->getStateName(), "Waiting");
}

TEST(ResetWorkflowCommandEdgeTest, Edge_WorkflowWithNoNodesStillResets) {
    WorkflowInstance empty("Survey-0001", "Survey", nullptr);
    empty.markRunning();

    EXPECT_TRUE(ResetWorkflowCommand(&empty).execute());
    EXPECT_EQ(empty.getCurrent(), nullptr);
}

TEST_F(ResetWorkflowCommandTest, Edge_CanBeExecutedTwice) {
    second->start(); second->reject();
    ResetWorkflowCommand cmd(&instance);

    EXPECT_TRUE(cmd.execute());
    EXPECT_TRUE(cmd.execute());  // children are rebuilt each time, not reused
    EXPECT_EQ(second->getStateName(), "Waiting");
}

// ---------------------------------------------------------------- Errors

TEST(ResetWorkflowCommandErrorsTest, Errors_NullInstanceReturnsFalseWithoutCrashing) {
    ResetWorkflowCommand cmd(nullptr);

    bool ok = true;
    EXPECT_NO_THROW(ok = cmd.execute());
    EXPECT_FALSE(ok);
    EXPECT_EQ(cmd.describe(), "Reset workflow (none)");
}
