#ifndef APPROVALREQUEST_H
#define APPROVALREQUEST_H

#include <string>
#include <vector>
#include "ApprovalDecision.h"
#include "ApprovalRecord.h"
#include "ApprovalPolicy.h"

/**
 * @see WorkflowInstance.h
 */
class WorkflowInstance;

/**
 * @brief defines what a request looks like and provides constructors in order to create a request
 */
struct ApprovalRequest
{
    public:
    //members
    /** @brief holds a pointer to the workflowinstance  */
    const WorkflowInstance* instance;

    /// @brief holds record of all of the approvers in a chain's decision on the request    
    std::vector<ApprovalRecord> records;

    /// @brief determines the policy by which Approvers handle requests
    ApprovalPolicy policy;

    /// @brief contains the final decision of a request: Approved or Rejected
    ApprovalDecision finalDecision;

    /// @brief indicates the approver who last updated the request's final decision
    std::string decidedBy;

    //functions
    /**
     * @brief default constructor for creating a request object
     * @param wi pointer to the workflowinstance
     * @param p policy by which the approvers handle requests
     */
    ApprovalRequest(const WorkflowInstance* wi, ApprovalPolicy p):
        instance(wi), records(), policy(p), finalDecision(ApprovalDecision::Rejected), decidedBy(){}

    /**
     * @brief full constructor which creates a request object
     * @param wi pointer to the workflowinstance
     * @param r vector that contains the records of the request
     * @param p policy by which the approvers handle requests
     * @param fD decision of the request
     * @param dB approver who updated the final decision of the request
     */
    ApprovalRequest(const WorkflowInstance* wi, const std::vector<ApprovalRecord>& r, ApprovalPolicy p, 
        ApprovalDecision fD, const std::string& dB):
        instance(wi), records(r), policy(p), finalDecision(fD), decidedBy(dB){}


};

#endif