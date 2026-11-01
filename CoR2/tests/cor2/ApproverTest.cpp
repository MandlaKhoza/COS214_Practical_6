#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <string>

#include "Approver.h"
#include "RuleBasedApprover.h"
#include "ApprovalRequest.h"

typedef std::function<ApprovalDecision(const ApprovalRequest&)> Rule;

/**
 * Test double: an approver that always returns the decision it was given,
 * and counts how many times the chain asked it.
 */
class FixedApprover : public Approver
{
    public:
        mutable int asked;   // mutable because decide() is const

        FixedApprover(const std::string& name, const std::string& role,
                      ApprovalDecision d)
            : Approver(name, role), asked(0), answer(d)
        {
        }

    protected:
        ApprovalDecision decide(const ApprovalRequest&) const override
        {
            ++asked;
            return answer;
        }

    private:
        ApprovalDecision answer;
};

/// Links 'next' after 'tail'. The chain takes ownership of 'next'.
static FixedApprover* append(Approver& tail, FixedApprover* next)
{
    tail.setNext(std::unique_ptr<Approver>(next));
    return next;
}

static ApprovalRequest makeRequest(ApprovalPolicy p)
{
    return ApprovalRequest(nullptr, p);   // no instance needed for these tests
}

// ---------- SequentialSignOff ----------

TEST(ApproverTest, SequentialSingleApproverApproves)
{
    FixedApprover a("Ann", "Clerk", ApprovalDecision::Approved);
    ApprovalRequest req = makeRequest(ApprovalPolicy::SequentialSignOff);

    a.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("Ann", req.decidedBy);
    EXPECT_EQ(1u, req.records.size());
}

TEST(ApproverTest, SequentialAllApproveEndsWithLastApprover)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Approved);
    FixedApprover* b = append(a, new FixedApprover("B", "Manager", ApprovalDecision::Approved));
    FixedApprover* c = append(*b, new FixedApprover("C", "HOD", ApprovalDecision::Approved));
    ApprovalRequest req = makeRequest(ApprovalPolicy::SequentialSignOff);

    a.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("C", req.decidedBy);
    EXPECT_EQ(3u, req.records.size());
    EXPECT_EQ(1, a.asked);
    EXPECT_EQ(1, b->asked);
    EXPECT_EQ(1, c->asked);
}

TEST(ApproverTest, SequentialMiddleRejectionStopsTheChain)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Approved);
    FixedApprover* b = append(a, new FixedApprover("B", "Manager", ApprovalDecision::Rejected));
    FixedApprover* c = append(*b, new FixedApprover("C", "HOD", ApprovalDecision::Approved));
    ApprovalRequest req = makeRequest(ApprovalPolicy::SequentialSignOff);

    a.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
    EXPECT_EQ("B", req.decidedBy);
    EXPECT_EQ(2u, req.records.size());
    EXPECT_EQ(0, c->asked);   // never reached
}

TEST(ApproverTest, SequentialFirstRejectionLeavesOneRecord)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Rejected);
    FixedApprover* b = append(a, new FixedApprover("B", "Manager", ApprovalDecision::Approved));
    ApprovalRequest req = makeRequest(ApprovalPolicy::SequentialSignOff);

    a.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
    EXPECT_EQ("A", req.decidedBy);
    EXPECT_EQ(1u, req.records.size());
    EXPECT_EQ(0, b->asked);
}

// ---------- EscalationByAuthority ----------

TEST(ApproverTest, EscalationFirstApprovalStopsImmediately)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Approved);
    FixedApprover* b = append(a, new FixedApprover("B", "Manager", ApprovalDecision::Approved));
    ApprovalRequest req = makeRequest(ApprovalPolicy::EscalationByAuthority);

    a.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("A", req.decidedBy);
    EXPECT_EQ(1u, req.records.size());
    EXPECT_EQ(0, b->asked);
}

TEST(ApproverTest, EscalationPassesUpUntilSomeoneApproves)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Escalate);
    FixedApprover* b = append(a, new FixedApprover("B", "Manager", ApprovalDecision::Approved));
    ApprovalRequest req = makeRequest(ApprovalPolicy::EscalationByAuthority);

    a.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Approved);
    EXPECT_EQ("B", req.decidedBy);
    EXPECT_EQ(2u, req.records.size());
    EXPECT_EQ(1, b->asked);
}

TEST(ApproverTest, EscalationWithNoAuthorityLeftIsRejected)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Escalate);
    FixedApprover* b = append(a, new FixedApprover("B", "Manager", ApprovalDecision::Escalate));
    ApprovalRequest req = makeRequest(ApprovalPolicy::EscalationByAuthority);

    a.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);   // never Escalate
    EXPECT_EQ("B", req.decidedBy);                                  // last one who looked
    ASSERT_EQ(2u, req.records.size());
    EXPECT_FALSE(req.records[1].comment.empty());
    EXPECT_TRUE(req.records[1].decision == ApprovalDecision::Escalate);
}

TEST(ApproverTest, EscalationRejectionStopsEvenIfLaterWouldApprove)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Rejected);
    FixedApprover* b = append(a, new FixedApprover("B", "Manager", ApprovalDecision::Approved));
    ApprovalRequest req = makeRequest(ApprovalPolicy::EscalationByAuthority);

    a.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
    EXPECT_EQ("A", req.decidedBy);
    EXPECT_EQ(0, b->asked);
}

// ---------- Safe defaults and records ----------

TEST(ApproverTest, NewRequestStartsRejectedAndEmpty)
{
    ApprovalRequest req = makeRequest(ApprovalPolicy::SequentialSignOff);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);   // fail-safe
    EXPECT_TRUE(req.records.empty());
    EXPECT_TRUE(req.decidedBy.empty());
    EXPECT_TRUE(req.instance == nullptr);
}

TEST(ApproverTest, EveryRecordHasAComment)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Approved);
    FixedApprover* b = append(a, new FixedApprover("B", "Manager", ApprovalDecision::Approved));
    append(*b, new FixedApprover("C", "HOD", ApprovalDecision::Approved));
    ApprovalRequest req = makeRequest(ApprovalPolicy::SequentialSignOff);

    a.approve(req);

    ASSERT_EQ(3u, req.records.size());
    for (std::size_t i = 0; i < req.records.size(); i++)
    {
        EXPECT_FALSE(req.records[i].comment.empty());
    }
}

TEST(ApproverTest, ApproveReturnsTheSameValueAsFinalDecision)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Escalate);
    append(a, new FixedApprover("B", "Manager", ApprovalDecision::Rejected));
    ApprovalRequest req = makeRequest(ApprovalPolicy::EscalationByAuthority);

    ApprovalDecision returned = a.approve(req);

    EXPECT_TRUE(returned == req.finalDecision);
    EXPECT_TRUE(returned == ApprovalDecision::Rejected);
}

TEST(ApproverTest, RecordsKeepChainOrderWithNamesRolesAndDecisions)
{
    FixedApprover a("Ann", "Clerk", ApprovalDecision::Approved);
    append(a, new FixedApprover("Bob", "Manager", ApprovalDecision::Approved));
    ApprovalRequest req = makeRequest(ApprovalPolicy::SequentialSignOff);

    a.approve(req);

    ASSERT_EQ(2u, req.records.size());
    EXPECT_EQ("Ann", req.records[0].approverName);
    EXPECT_EQ("Clerk", req.records[0].role);
    EXPECT_TRUE(req.records[0].decision == ApprovalDecision::Approved);
    EXPECT_EQ("Bob", req.records[1].approverName);
    EXPECT_EQ("Manager", req.records[1].role);
}

// ---------- RuleBasedApprover ----------

TEST(RuleBasedApproverTest, ReturnsWhatItsRuleReturns)
{
    // decide() is protected, so check through the record approve() writes
    ApprovalDecision all[3] = { ApprovalDecision::Approved,
                                ApprovalDecision::Rejected,
                                ApprovalDecision::Escalate };

    for (int i = 0; i < 3; i++)
    {
        ApprovalDecision wanted = all[i];
        RuleBasedApprover r("R", "Rule", [wanted](const ApprovalRequest&) { return wanted; });
        ApprovalRequest req = makeRequest(ApprovalPolicy::EscalationByAuthority);

        r.approve(req);

        ASSERT_EQ(1u, req.records.size());
        EXPECT_TRUE(req.records[0].decision == wanted);
    }
}

TEST(RuleBasedApproverTest, EmptyRuleFailsSafeToRejected)
{
    RuleBasedApprover r("R", "Rule", Rule());   // empty std::function
    ApprovalRequest req = makeRequest(ApprovalPolicy::EscalationByAuthority);

    r.approve(req);

    EXPECT_TRUE(req.finalDecision == ApprovalDecision::Rejected);
    ASSERT_EQ(1u, req.records.size());
    EXPECT_TRUE(req.records[0].decision == ApprovalDecision::Rejected);
}

TEST(RuleBasedApproverTest, RuleCanReadTheRequest)
{
    // approves only under EscalationByAuthority
    Rule rule = [](const ApprovalRequest& req)
    {
        return req.policy == ApprovalPolicy::EscalationByAuthority
                   ? ApprovalDecision::Approved
                   : ApprovalDecision::Rejected;
    };

    RuleBasedApprover r1("R", "Rule", rule);
    RuleBasedApprover r2("R", "Rule", rule);
    ApprovalRequest escalation = makeRequest(ApprovalPolicy::EscalationByAuthority);
    ApprovalRequest sequential = makeRequest(ApprovalPolicy::SequentialSignOff);

    r1.approve(escalation);
    r2.approve(sequential);

    EXPECT_TRUE(escalation.finalDecision == ApprovalDecision::Approved);
    EXPECT_TRUE(sequential.finalDecision == ApprovalDecision::Rejected);
}

// ---------- Chain structure ----------

TEST(ApproverTest, SetNextReturnsTheAddedApproverAndChainCanBeWalked)
{
    FixedApprover a("A", "Clerk", ApprovalDecision::Approved);
    FixedApprover* b = new FixedApprover("B", "Manager", ApprovalDecision::Approved);
    FixedApprover* c = new FixedApprover("C", "HOD", ApprovalDecision::Approved);

    Approver* addedB = a.setNext(std::unique_ptr<Approver>(b));
    Approver* addedC = b->setNext(std::unique_ptr<Approver>(c));

    EXPECT_TRUE(addedB == b);
    EXPECT_TRUE(addedC == c);
    EXPECT_TRUE(a.getNext() == b);
    EXPECT_TRUE(b->getNext() == c);
    EXPECT_TRUE(c->getNext() == nullptr);
    EXPECT_EQ("A", a.getName());
    EXPECT_EQ("Clerk", a.getRole());
}