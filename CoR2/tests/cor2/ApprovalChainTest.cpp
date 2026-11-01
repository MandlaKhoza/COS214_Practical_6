#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "ApprovalChain.h"
#include "Approver.h"
#include "WorkflowInstance.h"
#include "Document.h"

// Test double: always returns the decision it was given, and counts calls
class FixedApprover : public Approver
{
    public:
        mutable int asked;

        FixedApprover(const std::string& name, const std::string& role, ApprovalDecision d)
            : Approver(name, role), asked(0), answer(d) {}

    protected:
        ApprovalDecision decide(const ApprovalRequest&) const override
        {
            ++asked;
            return answer;
        }

    private:
        ApprovalDecision answer;
};

static std::shared_ptr<WorkflowInstance> makeInstance()
{
    std::shared_ptr<Document> doc = std::make_shared<Document>();
    return std::make_shared<WorkflowInstance>("I-1", "LoanRequest", doc);
}

TEST(ApprovalChainTest, ConstructorStoresPolicyAndSetPolicyChangesIt)
{
    ApprovalChain chain(ApprovalPolicy::SequentialSignOff);
    EXPECT_TRUE(chain.getPolicy() == ApprovalPolicy::SequentialSignOff);

    chain.setPolicy(ApprovalPolicy::EscalationByAuthority);
    EXPECT_TRUE(chain.getPolicy() == ApprovalPolicy::EscalationByAuthority);
}

TEST(ApprovalChainTest, NoHeadIsRejectedWithExplanatoryRecord)
{
    ApprovalChain chain(ApprovalPolicy::SequentialSignOff);
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest req = chain.submit(*wi);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
    ASSERT_EQ(1u, req.records.size());
    EXPECT_EQ("No approvers configured", req.records[0].comment);
    EXPECT_EQ("ApprovalChain", req.decidedBy);
}

TEST(ApprovalChainTest, SubmitStoresInstanceAndPolicyInRequest)
{
    ApprovalChain chain(ApprovalPolicy::EscalationByAuthority);
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest req = chain.submit(*wi);

    EXPECT_TRUE(req.instance == wi.get());
    EXPECT_TRUE(req.policy == ApprovalPolicy::EscalationByAuthority);
}

TEST(ApprovalChainTest, SingleApproverApproves)
{
    ApprovalChain chain(ApprovalPolicy::SequentialSignOff);
    chain.setHead(std::unique_ptr<Approver>(
        new FixedApprover("Ann", "Clerk", ApprovalDecision::Approved)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest req = chain.submit(*wi);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("Ann", req.decidedBy);
    EXPECT_EQ(1u, req.records.size());
}

TEST(ApprovalChainTest, SequentialAllApproveEndsWithLastApprover)
{
    ApprovalChain chain(ApprovalPolicy::SequentialSignOff);
    Approver* a = chain.setHead(std::unique_ptr<Approver>(
        new FixedApprover("A", "Helper", ApprovalDecision::Approved)));
    Approver* b = a->setNext(std::unique_ptr<Approver>(
        new FixedApprover("B", "Manager", ApprovalDecision::Approved)));
    b->setNext(std::unique_ptr<Approver>(
        new FixedApprover("C", "HOD", ApprovalDecision::Approved)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest req = chain.submit(*wi);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("C", req.decidedBy);
    EXPECT_EQ(3u, req.records.size());
}

TEST(ApprovalChainTest, MiddleRejectionStopsTheChain)
{
    ApprovalChain chain(ApprovalPolicy::SequentialSignOff);
    Approver* a = chain.setHead(std::unique_ptr<Approver>(
        new FixedApprover("A", "Helper", ApprovalDecision::Approved)));
    Approver* b = a->setNext(std::unique_ptr<Approver>(
        new FixedApprover("B", "Manager", ApprovalDecision::Rejected)));
    FixedApprover* c = new FixedApprover("C", "HOD", ApprovalDecision::Approved);
    b->setNext(std::unique_ptr<Approver>(c));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest req = chain.submit(*wi);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
    EXPECT_EQ("B", req.decidedBy);
    EXPECT_EQ(2u, req.records.size());
    EXPECT_EQ(0, c->asked);
}

TEST(ApprovalChainTest, EscalationWithNoAuthorityLeftIsRejectedNeverEscalate)
{
    ApprovalChain chain(ApprovalPolicy::EscalationByAuthority);
    Approver* a = chain.setHead(std::unique_ptr<Approver>(
        new FixedApprover("A", "Clerk", ApprovalDecision::Escalate)));
    a->setNext(std::unique_ptr<Approver>(
        new FixedApprover("B", "Manager", ApprovalDecision::Escalate)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest req = chain.submit(*wi);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
    EXPECT_EQ("B", req.decidedBy);
}

TEST(ApprovalChainTest, ChangingPolicyChangesTheOutcome)
{
    ApprovalChain chain(ApprovalPolicy::EscalationByAuthority);
    Approver* a = chain.setHead(std::unique_ptr<Approver>(
        new FixedApprover("A", "Clerk", ApprovalDecision::Approved)));
    a->setNext(std::unique_ptr<Approver>(
        new FixedApprover("B", "Manager", ApprovalDecision::Approved)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest first = chain.submit(*wi);
    chain.setPolicy(ApprovalPolicy::SequentialSignOff);
    ApprovalRequest second = chain.submit(*wi);

    EXPECT_EQ("A", first.decidedBy);
    EXPECT_EQ(1u, first.records.size());
    EXPECT_EQ("B", second.decidedBy);
    EXPECT_EQ(2u, second.records.size());
}

TEST(ApprovalChainTest, SetHeadReturnsAddedApproverAndReplacementIsUsed)
{
    ApprovalChain chain(ApprovalPolicy::SequentialSignOff);
    FixedApprover* first = new FixedApprover("First", "Clerk", ApprovalDecision::Rejected);
    Approver* returned = chain.setHead(std::unique_ptr<Approver>(first));
    EXPECT_TRUE(returned == first);

    chain.setHead(std::unique_ptr<Approver>(
        new FixedApprover("Second", "Manager", ApprovalDecision::Approved)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest req = chain.submit(*wi);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("Second", req.decidedBy);
}

TEST(ApprovalChainTest, TwoSubmitsGiveIndependentRequests)
{
    ApprovalChain chain(ApprovalPolicy::SequentialSignOff);
    chain.setHead(std::unique_ptr<Approver>(
        new FixedApprover("A", "Clerk", ApprovalDecision::Approved)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    ApprovalRequest r1 = chain.submit(*wi);
    ApprovalRequest r2 = chain.submit(*wi);

    EXPECT_EQ(1u, r1.records.size());
    EXPECT_EQ(1u, r2.records.size());
}