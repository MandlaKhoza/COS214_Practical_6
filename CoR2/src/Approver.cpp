#include "Approver.h"

#include <utility>

//constructor
Approver::Approver(const std::string& n, const std::string& r)
{
    this->name = n;
    this->role = r;
}

//build chain of approvers
Approver* Approver::setNext(std::unique_ptr<Approver> n)
{
    this->next = std::move(n);
    return this->next.get();
}

//approve, reject or escalate a request
ApprovalDecision Approver::approve(ApprovalRequest& req)
{
    std::string message = ""; //empty for now, filled in when decision is made

    //create new record
    ApprovalDecision decision = decide(req);
    ApprovalRecord record = ApprovalRecord(this->name, this->role, decision, message);
    req.records.push_back(record);

    //rejected, Approved, Escalate
    switch (decision)
    {
        case ApprovalDecision::Rejected://Rejected

            req.finalDecision = ApprovalDecision::Rejected;
            req.decidedBy = this->name;

            message = "Request rejected by " + this->role + " " + this->name;
            req.records.back().comment = message;

            return req.finalDecision;

           

        break;

        case ApprovalDecision::Approved://Approved

            if (req.policy == ApprovalPolicy::EscalationByAuthority)
            {
                req.finalDecision = ApprovalDecision::Approved;
                req.decidedBy = this->name;

                message = "Request approved by "+ this->role + " " + this->name;
                req.records.back().comment = message;

                return req.finalDecision;  

            } 

            else if (req.policy == ApprovalPolicy::SequentialSignOff)
            {
                if(this->next != nullptr)
                {
                    message = "Request signed off by "+ this->role + " " + this->name;
                    req.records.back().comment = message;

                    return this->next.get()->approve(req); 

                }
                else
                {
                    req.finalDecision = ApprovalDecision::Approved;
                    req.decidedBy = this->name;

                    message = "Request approved by "+ this->role + " " + this->name;
                    req.records.back().comment = message;       
                }
                return req.finalDecision;
            }
            else
            {
                return req.finalDecision;
            }
            

        break;

        case ApprovalDecision::Escalate://Escalate
            
            if(this->next != nullptr)
            {

                message = "Request escalated by "+ this->role + " " + this->name;
                req.records.back().comment = message;

                return this->next.get()->approve(req); 

            }
            else
            {
                req.finalDecision = ApprovalDecision::Rejected;
                req.decidedBy = this->name;

                message = "Can't escalate request further";
                req.records.back().comment = message;
            }
            return req.finalDecision;

        break;
        
        default:

            return ApprovalDecision::Rejected;

        break;
    }

    return ApprovalDecision::Rejected;

}

const std::string& Approver::getName() const
{
    return this->name;
}

const std::string& Approver::getRole() const
{
    return this->role;
}

Approver* Approver::getNext() const
{
    return this->next.get();
}





