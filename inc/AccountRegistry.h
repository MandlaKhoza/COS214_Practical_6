#ifndef ACCOUNTREGISTRY_H
#define ACCOUNTREGISTRY_H

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "Account.h"

class AccountRegistry{
private:
    std::map<std::string, std::unique_ptr<Account>> accounts;

public:
    void add(Account* account);
    Account* find(const std::string& accountNumber) const;
    std::vector<Account*> findByOwner(const std::string& ownerId) const;
};

#endif 