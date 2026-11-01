
#include "../inc/ApprovalNode.h"
#include "../inc/ApprovalChain.h"
#include "../../WorkflowInstance/inc/WorkflowInstance.h"
#include "../inc/ApprovalRequest.h"
#include "../inc/ApprovalDecision.h"
#include "approval_prompt.hpp"
#include "ui_logger.hpp"


/**
 * @brief Constructs an approval node.
 *
 * @param name The name of the workflow node.
 * @param approvals Shared pointer to the approval chain.
 */
ApprovalNode::ApprovalNode(
    const std::string& name,
    std::shared_ptr<ApprovalChain> approvals
)
    : WorkflowNode(name), approvals(approvals) {
}

/**
 * @brief Creates a copy of this approval node.
 *
 * The copied node shares the same approval chain.
 *
 * @return A unique pointer to the cloned workflow node.
 */
std::unique_ptr<WorkflowNode> ApprovalNode::clone() {
    return std::unique_ptr<WorkflowNode>(
        new ApprovalNode(getName(), approvals)
    );
}


NodeResult ApprovalNode::process(WorkflowInstance& wi)
{

    // Fail-safe: no chain configured means nobody can approve
    if (!approvals)
    {
        UI_LOG("[APPROVAL] No approval chain configured");
        return NodeResult{HandleResult::REJECTED, "No approval chain configured"};
    }

    needsApproval();
    UI_LOG("[APPROVAL] Pending human approval before evaluating faculty rules...");
    const bool humanApproved = ApprovalPrompt::instance().waitForDecision(
        "ApprovalNode '" + getName() + "' requires your decision. Yes continues to the configured approval rules."
    );
    if (!humanApproved) {
        UI_LOG("[APPROVAL] Human rejected the request");
        return NodeResult{HandleResult::REJECTED, "Rejected by human operator"};
    }
    UI_LOG("[APPROVAL] Human approved; evaluating configured approval rules");

    ApprovalRequest request = approvals->submit(wi);

    // Copy every approver's record into the instance history
    for (std::size_t i = 0; i < request.records.size(); i++)
    {
        wi.addHistory(request.records[i].approverName, request.records[i].comment);
    }

    // finalDecision is only ever Approved or Rejected
    if (request.finalDecision == ApprovalDecision::Approved)
    {

        UI_LOG("[APPROVAL] Approved by " + request.decidedBy + " and human operator");
        return NodeResult{HandleResult::PASSED,
                          "Approved by " + request.decidedBy + " and human operator"};
    }

    if (request.decidedBy.empty())
    {
        UI_LOG("[APPROVAL] Rejected: no approver decided");
        return NodeResult{HandleResult::REJECTED, "Rejected: no approver decided"};
    }
    UI_LOG("[APPROVAL] Rejected by " + request.decidedBy);
    return NodeResult{HandleResult::REJECTED, "Rejected by " + request.decidedBy};
}
