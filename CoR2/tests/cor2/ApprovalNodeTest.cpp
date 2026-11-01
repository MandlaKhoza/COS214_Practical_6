#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

#include "ApprovalNode.h"
#include "ApprovalChain.h"
#include "Approver.h"
#include "WorkflowInstance.h"
#include "Document.h"
#include "WorkflowObserver.h"

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

        NodeResult callProcess(WorkflowInstance& wi) {
            if (getStateName() != "Waiting") {
                reset();
            }
            start();
            return process(wi);
        }
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

class OutcomeRecorder : public WorkflowObserver {
public:
    std::vector<Outcome> outcomes;

    void onWorkflowFinished(WorkflowInstance&, Outcome outcome) override {
        outcomes.push_back(outcome);
    }
};

class ResultNode : public WorkflowNode {
public:
    ResultNode(const std::string& name, HandleResult result,
               bool throwError = false, bool waitForApproval = false)
        : WorkflowNode(name), result(result), throwError(throwError),
          waitForApproval(waitForApproval) {}

    std::unique_ptr<WorkflowNode> clone() override {
        return std::unique_ptr<WorkflowNode>(
            new ResultNode(getName(), result, throwError, waitForApproval));
    }

protected:
    NodeResult process(WorkflowInstance&) override {
        if (waitForApproval) {
            needsApproval();
        }
        if (throwError) {
            throw std::runtime_error("Service unavailable");
        }
        return NodeResult{result, "Test result"};
    }

private:
    HandleResult result;
    bool throwError;
    bool waitForApproval;
};

TEST(ApprovalNodeTest, ApprovedPendingNodePassesAndContinuesTheWorkflow)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance();
    std::shared_ptr<OutcomeRecorder> observer = std::make_shared<OutcomeRecorder>();
    wi->attach(observer);
    std::unique_ptr<WorkflowNode> approval(new ApprovalNode("Approval",
        chainOf(ApprovalPolicy::SequentialSignOff,
                new FixedApprover("Ann", "Clerk", ApprovalDecision::Approved))));
    approval->setNext(std::unique_ptr<WorkflowNode>(new ResultNode("AfterApproval", PASSED)));
    wi->setChain(std::move(approval));

    wi->run();

    EXPECT_EQ("Passed", wi->getChain()->getStateName());
    EXPECT_EQ("Passed", wi->getChain()->getNext()->getStateName());
    EXPECT_TRUE(wi->isFinished());
    EXPECT_FALSE(wi->isFailed());
    ASSERT_EQ(1u, observer->outcomes.size());
    EXPECT_EQ(Outcome::Approved, observer->outcomes[0]);
}

TEST(WorkflowFailureTest, ReturnedFailureTerminatesWithoutRejectingOrContinuing)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance();
    std::shared_ptr<OutcomeRecorder> observer = std::make_shared<OutcomeRecorder>();
    wi->attach(observer);
    std::unique_ptr<WorkflowNode> node(new ResultNode("ExternalService", FAILED));
    node->setNext(std::unique_ptr<WorkflowNode>(new ResultNode("Unreached", PASSED)));
    wi->setChain(std::move(node));

    wi->run();

    EXPECT_TRUE(wi->isFinished());
    EXPECT_TRUE(wi->isFailed());
    EXPECT_TRUE(wi->getRejectedAt().empty());
    EXPECT_EQ("Failed", wi->getCurrent()->getStateName());
    EXPECT_EQ("Waiting", wi->getChain()->getNext()->getStateName());
    ASSERT_EQ(1u, observer->outcomes.size());
    EXPECT_EQ(Outcome::Failed, observer->outcomes[0]);
}

TEST(WorkflowFailureTest, ThrownExceptionTerminatesAndNotifiesOnce)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance();
    std::shared_ptr<OutcomeRecorder> observer = std::make_shared<OutcomeRecorder>();
    wi->attach(observer);
    wi->setChain(std::unique_ptr<WorkflowNode>(new ResultNode("ExternalService", PASSED, true)));

    EXPECT_NO_THROW(wi->run());

    EXPECT_TRUE(wi->isFinished());
    EXPECT_TRUE(wi->isFailed());
    EXPECT_TRUE(wi->getRejectedAt().empty());
    EXPECT_EQ("Failed", wi->getCurrent()->getStateName());
    ASSERT_EQ(1u, observer->outcomes.size());
    EXPECT_EQ(Outcome::Failed, observer->outcomes[0]);
}

TEST(WorkflowFailureTest, ExceptionWhilePendingCanTransitionToFailed)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance();
    wi->setChain(std::unique_ptr<WorkflowNode>(new ResultNode("HumanApproval", PASSED, true, true)));

    EXPECT_NO_THROW(wi->run());

    EXPECT_TRUE(wi->isFinished());
    EXPECT_TRUE(wi->isFailed());
    EXPECT_EQ("Failed", wi->getCurrent()->getStateName());
}

TEST(ApprovalNodeTest, PendingRejectionStopsTheWorkflow)
{
    std::shared_ptr<WorkflowInstance> wi = makeInstance();
    std::shared_ptr<OutcomeRecorder> observer = std::make_shared<OutcomeRecorder>();
    wi->attach(observer);
    wi->setChain(std::unique_ptr<WorkflowNode>(new ResultNode("HumanApproval", REJECTED, false, true)));

    wi->run();

    EXPECT_TRUE(wi->isFinished());
    EXPECT_FALSE(wi->isFailed());
    EXPECT_EQ("HumanApproval", wi->getRejectedAt());
    EXPECT_EQ("Rejected", wi->getCurrent()->getStateName());
    ASSERT_EQ(1u, observer->outcomes.size());
    EXPECT_EQ(Outcome::Rejected, observer->outcomes[0]);
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