#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>

#include "ThresholdApprover.h"
#include "ApprovalRequest.h"
#include "WorkflowInstance.h"
#include "Document.h"

/// Builds an instance whose document has the given numeric fields
static std::shared_ptr<WorkflowInstance> makeInstance(
    const std::map<std::string, double>& numbers,
    const std::map<std::string, std::string>& text = std::map<std::string, std::string>())
{
    std::shared_ptr<Document> doc = std::make_shared<Document>();
    doc->setDocument(text, numbers);
    return std::make_shared<WorkflowInstance>("I-1", "LoanRequest", doc);
}

static std::map<std::string, double> loan(double amount)
{
    std::map<std::string, double> m;
    m["loanAmount"] = amount;
    return m;
}

TEST(ThresholdApproverTest, ValueBelowLimitIsApproved)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(loan(30000));
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    ASSERT_EQ(1u, req.records.size());
    EXPECT_TRUE(req.records[0].decision == ApprovalDecision::Approved);
    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("Ann", req.decidedBy);
}

TEST(ThresholdApproverTest, ValueEqualToLimitIsApproved)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(loan(50000));
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    ASSERT_EQ(1u, req.records.size());
    EXPECT_TRUE(req.records[0].decision == ApprovalDecision::Approved);
}

TEST(ThresholdApproverTest, ZeroIsAValidValueAndApproved)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(loan(0));
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    ASSERT_EQ(1u, req.records.size());
    EXPECT_TRUE(req.records[0].decision == ApprovalDecision::Approved);
}

TEST(ThresholdApproverTest, ValueAboveLimitEscalatesAndEndsRejectedWithNoNext)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(loan(120000));
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    ASSERT_EQ(1u, req.records.size());
    EXPECT_TRUE(req.records[0].decision == ApprovalDecision::Escalate);
    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);   // never Escalate
}

TEST(ThresholdApproverTest, AboveLimitIsPassedUpToAHigherAuthority)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(loan(120000));
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    officer.setNext(std::unique_ptr<Approver>(
        new ThresholdApprover("Bob", "Branch Manager", "loanAmount", 500000)));
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    ASSERT_EQ(2u, req.records.size());
    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("Bob", req.decidedBy);
}

TEST(ThresholdApproverTest, AboveEveryLimitIsRejected)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(loan(900000));
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    officer.setNext(std::unique_ptr<Approver>(
        new ThresholdApprover("Bob", "Branch Manager", "loanAmount", 500000)));
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
    EXPECT_EQ("Bob", req.decidedBy);
}

TEST(ThresholdApproverTest, MissingFieldIsRejected)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(std::map<std::string, double>());
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    ASSERT_EQ(1u, req.records.size());
    EXPECT_TRUE(req.records[0].decision == ApprovalDecision::Rejected);
    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
}

TEST(ThresholdApproverTest, MissingFieldDoesNotEscalateToALaterApprover)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(std::map<std::string, double>());
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    officer.setNext(std::unique_ptr<Approver>(
        new ThresholdApprover("Bob", "Branch Manager", "loanAmount", 500000)));
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    EXPECT_EQ(1u, req.records.size());   // bad data stops the chain
    EXPECT_EQ("Ann", req.decidedBy);
}

TEST(ThresholdApproverTest, TextFieldIsNotTreatedAsANumber)
{
    std::map<std::string, std::string> text;
    text["loanAmount"] = "30000";   // in the text map, not the numeric one
    std::shared_ptr<WorkflowInstance> wi = makeInstance(std::map<std::string, double>(), text);
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
}

TEST(ThresholdApproverTest, DifferentFieldNameIsUsed)
{
    std::map<std::string, double> m;
    m["refundAmount"] = 400;
    std::shared_ptr<WorkflowInstance> wi = makeInstance(m);
    ThresholdApprover agent("Cy", "Support Agent", "refundAmount", 500);
    ApprovalRequest req(wi.get(), ApprovalPolicy::EscalationByAuthority);

    agent.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
}

TEST(ThresholdApproverTest, NullInstanceIsRejected)
{
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    ApprovalRequest req(nullptr, ApprovalPolicy::EscalationByAuthority);

    officer.approve(req);

    ASSERT_EQ(1u, req.records.size());
    EXPECT_TRUE(req.records[0].decision == ApprovalDecision::Rejected);
    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
}

TEST(ThresholdApproverTest, SequentialSignOffNeedsEveryLevelWithinItsLimit)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance(loan(30000));
    ThresholdApprover officer("Ann", "Loan Officer", "loanAmount", 50000);
    officer.setNext(std::unique_ptr<Approver>(
        new ThresholdApprover("Bob", "Branch Manager", "loanAmount", 500000)));
    ApprovalRequest req(wi.get(), ApprovalPolicy::SequentialSignOff);

    officer.approve(req);

    EXPECT_EQ(2u, req.records.size());
    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("Bob", req.decidedBy);
}