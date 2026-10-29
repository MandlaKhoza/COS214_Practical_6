#ifndef GeneralAccountFactory_H
#define GeneralAccountFactory_H
#include <string>
#include <memory>

class GeneralAccountFactory : public AccountFactory{
public:
    GeneralAccountFactory(string prefix, string ownerField);

protected:
unique_ptr<Account> createAccount(WorkflowInstance : wi);
};
#endif