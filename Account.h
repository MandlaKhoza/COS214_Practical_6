#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <map>

class Account{
protected:
    std::string accountNumber;
    std::string workflowType;
    std::string ownerId;
    std::string createdAt;

public:
    Account(std::string number, std::string type, std::string ownerId,  std::string createdAt);
    std::string getAccountNumber() const;
    std::string getWorkflowType() const;
    std::string getOwnerId() const;
    std::string getCreatedAt() const;
    virtual std::string describe() const;
    virtual std::map<std::string, std::string> getDetails() const = 0;
};

#endif 