#ifndef THRESHOLDAPPROVER_H
#define THRESHOLDAPPROVER_H

#include "Approver.h"

/**
 * @brief Concrete approver that decides by comparing a numeric field of the document against a maximum authority limit
 */
class ThresholdApprover : public Approver
{
    //members
    private:
        /// @brief Name of the document field to compare, e.g. FieldNames::LOAN_AMOUNT
        std::string valueField;

        /// @brief The largest value this approver may approve on their own authority
        double maxAuth;

    //functions
    public:
        /**
         * @brief creates a Thresholdapprover with a name, role, value field and a max auth
         * @param n name of approver
         * @param r role of approver
         * @param vF Name of the document field whose number is compared against the limit
         * @param auth The maximum value this approver may approve; anything above it is escalated
         */
        ThresholdApprover(const std::string& n, const std::string& r, const std::string& vF, double auth);

    protected:
    /**
     * @brief Compares the document's value to the approver's authority limit
     * @param req the request that needs to be handled by the approver
     * @return returns the decision the approver made on the request, Approved if equal or below, escalate of above, rejected if missing
     */
        ApprovalDecision decide(const ApprovalRequest& req) const override;

};

#endif