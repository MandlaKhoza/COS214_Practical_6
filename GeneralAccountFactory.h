#ifndef GENERALACCOUNTFACTORY_H
#define GENERALACCOUNTFACTORY_H

#include "AccountFactory.h"

class GeneralAccountFactory : public AccountFactory{
public:
    GeneralAccountFactory(std::string prefix, std::string ownerField);
    std::unique_ptr<Account> createAccount(WorkflowInstance& w) override;
};

#endif 