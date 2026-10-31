#include "RuleBasedApprover.h"

RuleBasedApprover::RuleBasedApprover(const std::string& n, const std::string& ro, std::function<ApprovalDecision(const ApprovalRequest&)> ru) :
Approver(n, ro), rule(ru)
{
}

ApprovalDecision RuleBasedApprover::decide(const ApprovalRequest& req) const
{
    if(this->rule == nullptr)
    {
        return ApprovalDecision::Rejected;
    }
    else
    {
        return rule(req);
    }
}