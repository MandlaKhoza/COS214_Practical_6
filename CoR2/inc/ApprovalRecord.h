#ifndef APPROVALRECORD_H
#define APPROVALRECORD_H

#include <string>
#include "ApprovalDecision.h"

/**
 * @brief this is the definition of a record stored in a request
 * @see ApprovalRequest.h
 */
struct ApprovalRecord
{
    public:
    //members
    /// @brief approver that added the record
    std::string approverName;

    /// @brief role of the approver
    std::string role;

    /// @brief decision made by the approver
    ApprovalDecision decision;

    /// @brief comment that describes the decision
    std::string comment;

    //functions

    /**
     * @brief default constructor that sets everything empty except decision, which defaults to Rejected
     */
    ApprovalRecord():approverName(), role(), decision(ApprovalDecision::Rejected), comment(){}
    
    /**
     * @brief full constructor
     * @param aN name of the approver
     * @param r role of the approver
     * @param d decision made by the appover
     * @param c comment that describes the decision
     */
    ApprovalRecord(std::string aN, std::string r, ApprovalDecision d, std::string c): 
        approverName(aN), role(r), decision(d), comment(c){}
    
};

#endif