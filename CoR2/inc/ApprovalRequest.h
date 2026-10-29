#ifndef APPROVALREQUEST_H
#define APPROVALREQUEST_H

#include <string>
#include <vector>
#include "ApprovalDecision.h"
#include "ApprovalRecord.h"
#include "ApprovalPolicy.h"

class WorkflowInstance;

struct ApprovalRequest
{
    public:
    //members
    const WorkflowInstance* instance;
    std::vector<ApprovalRecord> records;
    ApprovalPolicy policy;
    ApprovalDecision finalDecision;
    std::string decidedBy;

    //functions
    ApprovalRequest(const WorkflowInstance* wi):
        instance(wi), records(), policy(), finalDecision(ApprovalDecision::Rejected), decidedBy(){}

    ApprovalRequest(const WorkflowInstance* wi, std::vector<ApprovalRecord> r, ApprovalPolicy p, ApprovalDecision fD, std::string dB):
        instance(wi), records(r), policy(p), finalDecision(fD), decidedBy(dB){}


};

#endif