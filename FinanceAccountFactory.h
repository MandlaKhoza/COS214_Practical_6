#ifndef FINANCEACCOUNTFACTORY_H
#define FINANCEACCOUNTFACTORY_H

#include "AccountFactory.h"

class FinanceAccountFactory : public AccountFactory{
public:
    FinanceAccountFactory(std::string prefix, std::string ownerField);
    std::unique_ptr<Account> createAccount(WorkflowInstance& w) override;
};

#endif 