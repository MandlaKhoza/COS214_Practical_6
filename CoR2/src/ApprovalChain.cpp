
#include "ApprovalChain.h"
#include "WorkflowInstance.h"
#include <utility>

/**
 * @brief Constructs an approval chain
 * @param policy The policy used to process approval requests
 */
ApprovalChain::ApprovalChain(ApprovalPolicy policy)
    : head(nullptr), policy(policy) {
}

/**
 * @brief Sets the first approver in the chain
 * @param newHead The first approver
 * @return Pointer to the first approver
 */
Approver* ApprovalChain::setHead(std::unique_ptr<Approver> newHead) {
    head = std::move(newHead);
    return head.get();
}

/**
 * @brief Changes the approval policy
 * @param newPolicy The new approval policy
 */
void ApprovalChain::setPolicy(ApprovalPolicy newPolicy) {
    policy = newPolicy;
}

/**
 * @brief Returns the current approval policy
 * @return The current ApprovalPolicy
 */
ApprovalPolicy ApprovalChain::getPolicy() const {
    return policy;
}

/**
 * @brief Submits a workflow instance for approval
 *
 * Creates an ApprovalRequest and passes it to the first
 * approver in the chain
 *
 * @param wi The workflow instance requiring approval
 * @return The completed approval request containing the decision
 *         and approver records
 */
ApprovalRequest ApprovalChain::submit(const WorkflowInstance& wi) {
    ApprovalRequest request;

    request.instance = &wi;
    request.policy = policy;
    request.finalDecision = ApprovalDecision::Escalate;
    request.decidedBy = "";

    // Start processing at the first approver
    if (head) {
        request.finalDecision = head->approve(request);
    }

    return request;
}
