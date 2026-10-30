#ifndef REALACCOUNT_H
#define REALACCOUNT_H

#include <vector>
#include "AccountView.h"

class Account;

class RealAccount : public AccountView
{
    //members
    private:
        const Account* account;
        std::vector<std::string> internalNodes;

    //functions
    public:
        RealAccount(const Account* a, std::vector<std::string> n);
        std::string display() const;
        std::string getAccountNumber() const;
        std::string displaySummary() const;
        const std::vector<std::string>& getInternalNodes() const;
        const std::string getOwnerId() const; 

};

#endif