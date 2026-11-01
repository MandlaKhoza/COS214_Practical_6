#ifndef RULEBASEDAPPROVER_H
#define RULEBASEDAPPROVER_H

/// @see Approver.h
#include "Approver.h"

#include <functional>

/**
 * @brief Concrete approver that uses a function to handle a request
 */
class RuleBasedApprover : public Approver
{
    //members
    private:
        /// @brief the rule that decides how a request gets handled
        std::function<ApprovalDecision(const ApprovalRequest&)> rule;

    //functions
    public:
        /** 
         * @brief constructs a rule based approver with a name, role and rule
         * @param n name of approver
         * @param ro role of approver
         * @param ru rule that the approver uses to handle a request
         */
        RuleBasedApprover(const std::string& n, const std::string& ro, std::function<ApprovalDecision(const ApprovalRequest&)> ru);

    protected:
    /**
     * @brief calls the rule on the request
     * @param req request that must be handled by the approver
     * @return the approver's decision on the passed in req is returned
     */
        ApprovalDecision decide(const ApprovalRequest& req) const override;

};

#endif