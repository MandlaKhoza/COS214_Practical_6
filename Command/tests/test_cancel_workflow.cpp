/**
 * @file test_cancel_workflow.cpp
 * @brief GoogleTest unit tests for CancelWorkflowCommand (Command pattern), run
 *        against the real WorkflowInstance.
 *
 * Each test is Arrange / Act / Assert, and test names start with the kind of case:
 * Positive, Negative, Edge or Errors.
 *
 * @author Hafani Ramagoma
 * @date 2026-10-31
 */
#include <gtest/gtest.h>

#include <memory>

#include "CancelWorkflowCommand.h"
#include "ResetWorkflowCommand.h"
#include "WorkflowController.h"
#include "WorkflowInstance.h"

/// @brief Fixture: one running loan workflow (no document or chain needed to cancel it).
class CancelWorkflowCommandTest : public ::testing::Test {
protected:
    void SetUp() override { instance.markRunning(); }
    WorkflowInstance instance{"LoanRequest-0007", "Loan", nullptr};
};

TEST_F(CancelWorkflowCommandTest, Positive_RunningWorkflowBecomesCancelled) {
    // Arrange
    CancelWorkflowCommand cmd(&instance);
    ASSERT_FALSE(instance.isCancelled());
    // Act
    bool ok = cmd.execute();
    // Assert
    EXPECT_TRUE(ok);
    EXPECT_TRUE(instance.isCancelled());
}

TEST_F(CancelWorkflowCommandTest, Positive_DescribeNamesTheWorkflow) {
    CancelWorkflowCommand cmd(&instance);
    EXPECT_EQ(cmd.describe(), "Cancel workflow LoanRequest-0007");
}

TEST_F(CancelWorkflowCommandTest, Positive_ControllerLogsTheCancelAsSucceeded) {
    WorkflowController controller;

    bool ok = controller.execute(std::unique_ptr<Command>(new CancelWorkflowCommand(&instance)));

    EXPECT_TRUE(ok);
    ASSERT_EQ(controller.getAuditLog().size(), 1u);
    EXPECT_EQ(controller.getAuditLog()[0].description, "Cancel workflow LoanRequest-0007");
    EXPECT_TRUE(controller.getAuditLog()[0].succeeded);
}

TEST_F(CancelWorkflowCommandTest, Negative_CancelledWorkflowCannotBeResetAfterwards) {
    // Cancelled is final: the reset that follows must be refused
    CancelWorkflowCommand(&instance).execute();

    ResetWorkflowCommand reset(&instance);
    EXPECT_FALSE(reset.execute());
    EXPECT_TRUE(instance.isCancelled());
}

TEST_F(CancelWorkflowCommandTest, Edge_CancellingTwiceIsHarmless) {
    CancelWorkflowCommand cmd(&instance);

    EXPECT_TRUE(cmd.execute());
    EXPECT_TRUE(cmd.execute());
    EXPECT_TRUE(instance.isCancelled());
}

TEST_F(CancelWorkflowCommandTest, Edge_WorkflowThatNeverStartedCanBeCancelled) {
    WorkflowInstance fresh("LoanRequest-0008", "Loan", nullptr);  // still Created

    EXPECT_TRUE(CancelWorkflowCommand(&fresh).execute());
    EXPECT_TRUE(fresh.isCancelled());
}

TEST(CancelWorkflowCommandErrorsTest, Errors_NullInstanceReturnsFalseWithoutCrashing) {
    CancelWorkflowCommand cmd(nullptr);

    bool ok = true;
    EXPECT_NO_THROW(ok = cmd.execute());
    EXPECT_FALSE(ok);
    EXPECT_EQ(cmd.describe(), "Cancel workflow (none)");
}
