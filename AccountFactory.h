#ifndef ACCOUNTFACTORY_H
#define ACCOUNTFACTORY_H
#include <string>
#include <memory>
#include "Account.h"

class WorkflowInstance; 

class AccountFactory{
protected:
    std::string prefix;
    int counter;
    std::string ownerField;

    std::unique_ptr<Account> create(WorkflowInstance& w);
    std::string nextAccountNumber();
    std::string getOwnerId(const WorkflowInstance& w) const;

public:
    AccountFactory(std::string prefix, std::string ownerField);
    virtual std::unique_ptr<Account> createAccount(WorkflowInstance& w) = 0;
    virtual ~AccountFactory();

};

#endif 