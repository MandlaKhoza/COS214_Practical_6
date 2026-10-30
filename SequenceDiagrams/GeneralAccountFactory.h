#ifndef GeneralAccountFactory_H
#define GeneralAccountFactory_H
#include <string>
#include <memory>
#include "AccountFactroy.h"

class GeneralAccountFactory : public AccountFactory {
public:
    GeneralAccountFactory(std::string prefix, std::string ownerField);

protected:
    std::unique_ptr<Account> createAccount(WorkflowInstance& wi) override;
};

#endif