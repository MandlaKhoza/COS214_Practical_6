#ifndef APPROVER_H
#define APPROVER_H

#include <string>
#include <memory>
#include "ApprovalRequest.h"
#include "ApprovalDecision.h"

class Approver
{
    //members
    private:
        std::string name;
        std::string role;
        std::unique_ptr<Approver> next;

    //functions
    public:
        Approver(const std::string& n, const std::string& r);
        virtual ~Approver() = default;
        Approver* setNext(std::unique_ptr<Approver> n);
        ApprovalDecision approve(ApprovalRequest& req);

    protected:
        virtual ApprovalDecision decide(const ApprovalRequest& req) const = 0;

    public:
        const std::string& getName() const;
        const std::string& getRole() const;
        Approver* getNext() const;
};

#endif