#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>

#include "ApprovalNode.h"
#include "ApprovalChain.h"
#include "Approver.h"
#include "WorkflowInstance.h"
#include "Document.h"

// Test double: always returns the decision it was given
class FixedApprover : public Approver
{
    public:
        FixedApprover(const std::string& name, const std::string& role, ApprovalDecision d)
            : Approver(name, role), answer(d) {}

    protected:
        ApprovalDecision decide(const ApprovalRequest&) const override
        {
            return answer;
        }

    private:
        ApprovalDecision answer;
};

// process() is protected, so expose it for testing
class TestableApprovalNode : public ApprovalNode
{
    public:
        TestableApprovalNode(const std::string& n, std::shared_ptr<ApprovalChain> a)
            : ApprovalNode(n, a) {}

        NodeResult callProcess(WorkflowInstance& wi) { return process(wi); }
};

static std::shared_ptr<WorkflowInstance> makeInstance()
{
    std::shared_ptr<Document> doc = std::make_shared<Document>();
    return std::make_shared<WorkflowInstance>("I-1", "LoanRequest", doc);
}

static bool contains(const std::string& text, const std::string& part)
{
    return text.find(part) != std::string::npos;
}

static std::shared_ptr<ApprovalChain> chainOf(ApprovalPolicy p, Approver* head)
{
    std::shared_ptr<ApprovalChain> chain = std::make_shared<ApprovalChain>(p);
    if (head != nullptr)
    {
        chain->setHead(std::unique_ptr<Approver>(head));
    }
    return chain;
}

TEST(ApprovalNodeTest, ApprovedChainGivesPassed)
{
    TestableApprovalNode node("Approval", chainOf(ApprovalPolicy::SequentialSignOff,
        new FixedApprover("Ann", "Clerk", ApprovalDecision::Approved)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    NodeResult r = node.callProcess(*wi);

    EXPECT_TRUE(r.result == PASSED);
    EXPECT_TRUE(contains(r.name, "Approved by Ann"));
}

TEST(ApprovalNodeTest, RejectedChainGivesRejectedAndNamesTheApprover)
{
    TestableApprovalNode node("Approval", chainOf(ApprovalPolicy::SequentialSignOff,
        new FixedApprover("Bob", "Manager", ApprovalDecision::Rejected)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    NodeResult r = node.callProcess(*wi);

    EXPECT_TRUE(r.result == REJECTED);
    EXPECT_TRUE(contains(r.name, "Rejected by Bob"));
}

TEST(ApprovalNodeTest, NullChainFailsSafeToRejected)
{
    TestableApprovalNode node("Approval", std::shared_ptr<ApprovalChain>());
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    NodeResult r = node.callProcess(*wi);

    EXPECT_TRUE(r.result == REJECTED);
    EXPECT_TRUE(contains(r.name, "No approval chain configured"));
}

TEST(ApprovalNodeTest, ChainWithNoHeadIsRejectedAndHistoryExplainsWhy)
{
    TestableApprovalNode node("Approval", chainOf(ApprovalPolicy::SequentialSignOff, nullptr));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    NodeResult r = node.callProcess(*wi);

    EXPECT_TRUE(r.result == REJECTED);
    ASSERT_EQ(1u, wi->getHistory().size());
    EXPECT_EQ("No approvers configured", wi->getHistory()[0].message);
}

TEST(ApprovalNodeTest, EveryApproverRecordIsCopiedIntoHistoryInOrder)
{
    std::shared_ptr<ApprovalChain> chain =
        std::make_shared<ApprovalChain>(ApprovalPolicy::SequentialSignOff);
    Approver* a = chain->setHead(std::unique_ptr<Approver>(
        new FixedApprover("Ann", "Helper", ApprovalDecision::Approved)));
    a->setNext(std::unique_ptr<Approver>(
        new FixedApprover("Bob", "Manager", ApprovalDecision::Approved)));
    TestableApprovalNode node("Approval", chain);
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    node.callProcess(*wi);

    ASSERT_EQ(2u, wi->getHistory().size());
    EXPECT_EQ("Ann", wi->getHistory()[0].nodeName);
    EXPECT_EQ("Bob", wi->getHistory()[1].nodeName);
    EXPECT_FALSE(wi->getHistory()[0].message.empty());
}

TEST(ApprovalNodeTest, EscalationWithNoAuthorityLeftIsNeverPassed)
{
    TestableApprovalNode node("Approval", chainOf(ApprovalPolicy::EscalationByAuthority,
        new FixedApprover("Ann", "Clerk", ApprovalDecision::Escalate)));
    std::shared_ptr<WorkflowInstance> wi = makeInstance();

    NodeResult r = node.callProcess(*wi);

    EXPECT_TRUE(r.result == REJECTED);
    EXPECT_FALSE(r.result == PASSED);
}

TEST(ApprovalNodeTest, CloneReturnsApprovalNodeWithSameName)
{
    TestableApprovalNode node("Approval", chainOf(ApprovalPolicy::SequentialSignOff,
        new FixedApprover("Ann", "Clerk", ApprovalDecision::Approved)));

    std::unique_ptr<WorkflowNode> copy = node.clone();

    ASSERT_TRUE(copy.get() != nullptr);
    EXPECT_EQ("Approval", copy->getName());
    EXPECT_TRUE(dynamic_cast<ApprovalNode*>(copy.get()) != nullptr);
}

TEST(ApprovalNodeTest, CloneSharesTheSameApprovalChain)
{
    std::shared_ptr<ApprovalChain> chain = chainOf(ApprovalPolicy::SequentialSignOff,
        new FixedApprover("Ann", "Clerk", ApprovalDecision::Approved));
    long before = chain.use_count();

    TestableApprovalNode node("Approval", chain);
    std::unique_ptr<WorkflowNode> copy = node.clone();

    // original node + clone each hold the chain
    EXPECT_EQ(before + 2, chain.use_count());
}

TEST(ApprovalNodeTest, TwoInstancesGetIndependentHistories)
{
    TestableApprovalNode node("Approval", chainOf(ApprovalPolicy::SequentialSignOff,
        new FixedApprover("Ann", "Clerk", ApprovalDecision::Approved)));
    std::shared_ptr<WorkflowInstance> first = makeInstance();
    std::shared_ptr<WorkflowInstance> second = makeInstance();

    node.callProcess(*first);
    node.callProcess(*first);
    node.callProcess(*second);

    EXPECT_EQ(2u, first->getHistory().size());
    EXPECT_EQ(1u, second->getHistory().size());
}