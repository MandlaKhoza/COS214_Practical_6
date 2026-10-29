#ifndef APPROVALRECORD_H
#define APPROVALRECORD_H

#include <string>
#include "ApprovalDecision.h"

struct ApprovalRecord
{
    public:
    //members
    std::string approverName;
    std::string role;
    ApprovalDecision decision;
    std::string comment;

    //functions
    ApprovalRecord():approverName(), role(), decision(ApprovalDecision::Approved), comment(){}
    ApprovalRecord(std::string aN, std::string r, ApprovalDecision d, std::string c): 
        approverName(aN), role(r), decision(d), comment(c){}
    
};

#endif