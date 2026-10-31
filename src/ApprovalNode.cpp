
#include "ApprovalNode.h"
#include "ApprovalChain.h"
#include "WorkflowInstance.h"
#include "ApprovalRequest.h"
#include "ApprovalDecision.h"

#include <stdexcept>

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
std::unique_ptr<WorkflowNode> ApprovalNode::clone() const {
    return std::unique_ptr<WorkflowNode>(
        new ApprovalNode(getName(), approvals)
    );
}

/**
 * @brief Processes the approval request.
 *
 * Submits the workflow instance to CHOR2 and converts
 * the resulting ApprovalDecision into a NodeResult.
 *
 * @param wi The workflow instance being processed.
 * @return NodeResult containing the outcome and history message.
 * @throws std::logic_error If no approval chain is assigned.
 */
NodeResult ApprovalNode::process(WorkflowInstance& wi) {
    if (!approvals) {
        throw std::logic_error("ApprovalNode has no ApprovalChain");
    }

    // Submit the workflow to the approval chain.
    ApprovalRequest request = approvals->submit(wi);

    
    switch (request.finalDecision) {

        case ApprovalDecision::Approved: //decision approved 
            return NodeResult{
                HandleResult::Passed,
                "Approved by " + request.decidedBy
            };

        case ApprovalDecision::Rejected: //decision rejected
            return NodeResult{
                HandleResult::Rejected,
                "Rejected by " + request.decidedBy
            };

        case ApprovalDecision::Escalate: //de
            return NodeResult{
                HandleResult::Pending,
                "Awaiting approval"
            };
    }

    throw std::logic_error("Unknown approval decision");
}
