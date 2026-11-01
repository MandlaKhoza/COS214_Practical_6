#ifndef APPROVALCHAIN_H
#define APPROVALCHAIN_H

#include <memory>
#include "ApprovalPolicy.h"
#include "ApprovalRequest.h"
#include "Approver.h"



class WorkflowInstance;

/**
 * @brief Holder and entry point of Chain of Responsibility 2.
 *
 * Owns the head of the approver chain and the approval policy.
 * submit() builds an ApprovalRequest, runs it through the approvers and
 * returns the filled-in request.
 * - The policy decides whether every approver must sign off in order
 *   (SequentialSignOff) or the first approver with enough authority
 *   decides (EscalationByAuthority).
 * - The head owns the whole chain through each approver's next pointer.
 */
class ApprovalChain {

private:
    /// @brief First approver in the chain; owns the rest of the chain.
	std::unique_ptr<Approver> head;

    /// @brief Whether approvers sign off in sequence or escalate by authority.
	ApprovalPolicy policy;

public:
    /**
    * @brief Creates an approval chain with a policy and no approvers.
    * @param p  The policy used for every request.
    */
	ApprovalChain(ApprovalPolicy p);

    /**
    * @brief Sets the first approver of the chain.
    * @param h  The first approver; ownership passes to the chain, so pass it with std::move.
    * @return A borrowed pointer to the approver just added, for chaining setNext().
    * @note Replaces and destroys any existing chain.
    */
	Approver* setHead(std::unique_ptr<Approver> h);

    /**
    * @brief Changes the approval policy.
    * @param p  The new policy.
    */
    void setPolicy(ApprovalPolicy p);

    /**
    * @brief Gets the approval policy.
    * @return The current policy.
    */
    ApprovalPolicy getPolicy() const;


    /**
    * @brief Runs a workflow instance through the approvers.
    * @param wi  The instance being approved; read only.
    * @return The filled-in request. finalDecision is only ever Approved or Rejected,
    *         records holds one entry per approver who looked at it, and decidedBy
    *         names the approver who ended the chain.
    * @note With no approvers configured the request stays Rejected, with a record
    *       saying so.
    */
    ApprovalRequest submit(const WorkflowInstance& wi) const;
};

#endif
