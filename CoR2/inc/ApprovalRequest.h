#ifndef APPROVALREQUEST
#define APPROVALREQUEST

#include <string>
#include <vector>
#include "ApprovalDecision.h"
#include "ApprovalRecord.h"
#include "ApprovalPolicy.h"

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
    ApprovalRequest(const WorkflowInstance* wi, std::vector<ApprovalRecord> r, ApprovalPolicy p, ApprovalDecision fD, std::string dB):
        instance(wi), records(r), policy(p), finalDecision(fD), decidedBy(dB){}


};

#endif