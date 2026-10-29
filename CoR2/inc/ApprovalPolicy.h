#ifndef APPROVALPOLICY_H
#define APPROVALPOLICY_H

/**
 * @brief the 2 types of approval policies
 */
enum class ApprovalPolicy
{
    /// @brief approves and passes on or rejects
    SequentialSignOff,

    /// @brief rejects or escalates
    EscalationByAuthority
};

#endif