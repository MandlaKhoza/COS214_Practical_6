#include "ThresholdApprover.h"
#include "WorkflowInstance.h"

ThresholdApprover::ThresholdApprover(const std::string& n, const std::string& r, const std::string& vF, double auth) :
    Approver(n, r), valueField(vF), maxAuth(auth)
{

}

ApprovalDecision ThresholdApprover::decide(const ApprovalRequest& req) const
{
    double value;

    if (req.instance == nullptr)
    {
        return ApprovalDecision::Rejected;
    }
    else
    {
        if (req.instance->getDocument().getNumber(valueField, value))
        {
            if (value<=maxAuth)
            {
                return ApprovalDecision::Approved;
            }
            else
            {
                return ApprovalDecision::Escalate;
            }
        }
        else
        {
            return ApprovalDecision::Rejected;
        }
    }
}