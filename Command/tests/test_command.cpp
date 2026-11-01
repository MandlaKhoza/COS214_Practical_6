/**
 * @file test_command.cpp
 * @brief GoogleTest unit tests for the Command pattern: the Command interface and
 *        the WorkflowController (Invoker).
 *
 * The real commands need WorkflowInstance and WorkflowNode, so these tests use a
 * FakeCommand (a mock) that does exactly what each test tells it to. That way the
 * Invoker is tested on its own. Each test is Arrange / Act / Assert, and test names
 * start with the kind of case: Positive, Negative, Edge or Errors.
 *
 * @author Hafani Ramagoma
 * @date 2026-10-31
 */
#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <string>

#include "Command.h"
#include "CommandLogEntry.h"
#include "WorkflowController.h"

namespace {

/**
 * @brief Mock Command: returns, or throws, whatever the test sets up, and records
 *        what happened to it so the test can check.
 */
class FakeCommand : public Command {
public:
    /// What execute() should do.
    enum class Behaviour { Succeed, Refuse, ThrowLogicError, ThrowInvalidArgument, ThrowRuntimeError };

    /**
     * @param[in]  text       What describe() returns.
     * @param[in]  behaviour  What execute() does.
     * @param[out] executed   Set to true when execute() runs (may be nullptr).
     * @param[out] destroyed  Set to true when this object is deleted (may be nullptr).
     */
    FakeCommand(const std::string& text, Behaviour behaviour,
                bool* executed = nullptr, bool* destroyed = nullptr)
        : text(text), behaviour(behaviour), executed(executed), destroyed(destroyed) {}

    ~FakeCommand() override {
        if (destroyed) *destroyed = true;
    }

    bool execute() override {
        if (executed) *executed = true;
        switch (behaviour) {
            case Behaviour::Succeed:              return true;
            case Behaviour::Refuse:               return false;
            case Behaviour::ThrowLogicError:      throw std::logic_error("Cannot reset a cancelled workflow");
            case Behaviour::ThrowInvalidArgument: throw std::invalid_argument("No answers given");
            case Behaviour::ThrowRuntimeError:    throw std::runtime_error("Database offline");
        }
        return false;
    }

    std::string describe() const override { return text; }

private:
    std::string text;
    Behaviour behaviour;
    bool* executed;
    bool* destroyed;
};

/// @brief Shortcut: makes a FakeCommand already wrapped in the unique_ptr execute() takes.
std::unique_ptr<Command> fake(const std::string& text, FakeCommand::Behaviour behaviour,
                              bool* executed = nullptr, bool* destroyed = nullptr) {
    return std::unique_ptr<Command>(new FakeCommand(text, behaviour, executed, destroyed));
}

}  // namespace

// =====================================================================================
//                                  COMMAND (interface)
// =====================================================================================

TEST(CommandTest, Positive_ConcreteCommandCanBeUsedThroughCommandPointer) {
    // Arrange
    std::unique_ptr<Command> cmd = fake("Cancel workflow LoanRequest-0001", FakeCommand::Behaviour::Succeed);
    // Act + Assert: only the Command interface is used here
    EXPECT_EQ(cmd->describe(), "Cancel workflow LoanRequest-0001");
    EXPECT_TRUE(cmd->execute());
}

TEST(CommandTest, Edge_DeletingThroughCommandPointerRunsDerivedDestructor) {
    // Arrange
    bool destroyed = false;
    std::unique_ptr<Command> cmd = fake("x", FakeCommand::Behaviour::Succeed, nullptr, &destroyed);
    // Act
    cmd.reset();
    // Assert: proves ~Command() is virtual (otherwise ~FakeCommand would be skipped)
    EXPECT_TRUE(destroyed);
}

// =====================================================================================
//                               WORKFLOWCONTROLLER (Invoker)
// =====================================================================================

/// @brief Fixture: a fresh controller for every test.
class WorkflowControllerTest : public ::testing::Test {
protected:
    WorkflowController controller;
};

TEST_F(WorkflowControllerTest, Positive_NewControllerHasEmptyLog) {
    EXPECT_TRUE(controller.getAuditLog().empty());
}

TEST_F(WorkflowControllerTest, Positive_SuccessfulCommandRunsReturnsTrueAndIsLogged) {
    // Arrange
    bool executed = false;
    // Act
    bool ok = controller.execute(fake("Reset workflow LoanRequest-0007",
                                      FakeCommand::Behaviour::Succeed, &executed));
    // Assert
    EXPECT_TRUE(executed);
    EXPECT_TRUE(ok);
    ASSERT_EQ(controller.getAuditLog().size(), 1u);
    EXPECT_EQ(controller.getAuditLog()[0].description, "Reset workflow LoanRequest-0007");
    EXPECT_TRUE(controller.getAuditLog()[0].succeeded);
}

TEST_F(WorkflowControllerTest, Positive_LogKeepsEveryCommandOldestFirst) {
    controller.execute(fake("Cancel workflow A", FakeCommand::Behaviour::Succeed));
    controller.execute(fake("Reset workflow B", FakeCommand::Behaviour::Refuse));
    controller.execute(fake("Appeal workflow C", FakeCommand::Behaviour::Succeed));

    const std::vector<CommandLogEntry>& log = controller.getAuditLog();
    ASSERT_EQ(log.size(), 3u);
    EXPECT_EQ(log[0].description, "Cancel workflow A");
    EXPECT_EQ(log[1].description, "Reset workflow B");
    EXPECT_EQ(log[2].description, "Appeal workflow C");
}

TEST_F(WorkflowControllerTest, Negative_CommandReturningFalseIsLoggedAsFailed) {
    bool ok = controller.execute(fake("Cancel workflow LoanRequest-0003", FakeCommand::Behaviour::Refuse));

    EXPECT_FALSE(ok);
    ASSERT_EQ(controller.getAuditLog().size(), 1u);
    EXPECT_FALSE(controller.getAuditLog()[0].succeeded);
    EXPECT_EQ(controller.getAuditLog()[0].description, "Cancel workflow LoanRequest-0003");  // no reason added
}

TEST_F(WorkflowControllerTest, Negative_NullCommandReturnsFalseAndIsNotLogged) {
    bool ok = controller.execute(std::unique_ptr<Command>());

    EXPECT_FALSE(ok);
    EXPECT_TRUE(controller.getAuditLog().empty());
}

TEST_F(WorkflowControllerTest, Errors_LogicErrorIsCaughtAndLoggedWithReason) {
    // Act: a state refusing the action throws std::logic_error
    bool ok = false;
    EXPECT_NO_THROW(ok = controller.execute(fake("Reset workflow LoanRequest-0003",
                                                 FakeCommand::Behaviour::ThrowLogicError)));
    // Assert
    EXPECT_FALSE(ok);
    ASSERT_EQ(controller.getAuditLog().size(), 1u);
    EXPECT_EQ(controller.getAuditLog()[0].description,
              "Reset workflow LoanRequest-0003 (refused: Cannot reset a cancelled workflow)");
    EXPECT_FALSE(controller.getAuditLog()[0].succeeded);
}

TEST_F(WorkflowControllerTest, Edge_SubclassOfLogicErrorIsAlsoCaught) {
    // std::invalid_argument derives from std::logic_error, so it is a refusal too
    bool ok = controller.execute(fake("Appeal workflow C", FakeCommand::Behaviour::ThrowInvalidArgument));

    EXPECT_FALSE(ok);
    ASSERT_EQ(controller.getAuditLog().size(), 1u);
    EXPECT_EQ(controller.getAuditLog()[0].description, "Appeal workflow C (refused: No answers given)");
}

TEST_F(WorkflowControllerTest, Errors_OtherExceptionsAreNotSwallowedAndNotLogged) {
    // A runtime_error is a real fault, not a refusal: the controller lets it through
    EXPECT_THROW(controller.execute(fake("Cancel workflow A", FakeCommand::Behaviour::ThrowRuntimeError)),
                 std::runtime_error);
    EXPECT_TRUE(controller.getAuditLog().empty());
}

TEST_F(WorkflowControllerTest, Edge_ControllerStillWorksAfterARefusal) {
    controller.execute(fake("Reset workflow A", FakeCommand::Behaviour::ThrowLogicError));
    bool ok = controller.execute(fake("Cancel workflow A", FakeCommand::Behaviour::Succeed));

    EXPECT_TRUE(ok);
    ASSERT_EQ(controller.getAuditLog().size(), 2u);
    EXPECT_TRUE(controller.getAuditLog()[1].succeeded);
}

TEST_F(WorkflowControllerTest, Edge_ControllerDeletesCommandAfterRunningIt) {
    // Arrange
    bool destroyed = false;
    // Act
    controller.execute(fake("Cancel workflow A", FakeCommand::Behaviour::Succeed, nullptr, &destroyed));
    // Assert: execute() owns the command, so it is gone once execute() returns (no leak)
    EXPECT_TRUE(destroyed);
}

TEST_F(WorkflowControllerTest, Edge_CommandIsDeletedEvenWhenRefused) {
    bool destroyed = false;
    controller.execute(fake("Reset workflow A", FakeCommand::Behaviour::ThrowLogicError, nullptr, &destroyed));
    EXPECT_TRUE(destroyed);
}
