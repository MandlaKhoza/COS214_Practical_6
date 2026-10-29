#ifndef APPROVALDECISION_H
#define APPROVALDECISION_H

/**
 * @brief the different types of decisions approvers can make
 */
enum class ApprovalDecision
{
    /// @brief approver approved the request
    Approved,

    /// @brief approver escalated request to the next approver, only usable by EscalationByAuthority policy 
    Escalate,

    /// @brief approver rejected the request
    Rejected
};

#endif