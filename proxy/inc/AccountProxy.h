#ifndef ACCOUNTPROXY_H
#define ACCOUNTPROXY_H

#include <memory>

#include "AccountView.h"
#include "AccessLog.h"
#include "RealAccount.h"

class AccountRegistry;

class AccountProxy : public AccountProxy
{
    //members
    private:
        std::shared_ptr<AccountRegistry> registry;
        std::string accountNumber;
        Requester requester;
        std::shared_ptr<AccessLog> log;
        std::unique_ptr<RealAccount> real;

    //functions
    public:
        AccountProxy(std::shared_ptr<AccountRegistry> r, std::string aN, Requester req, std::shared_ptr<AccessLog> l);
        std::string display() const;
        std::string getAccountNumber() const;
        AccessLevel determineAccess() const;
        RealAccount& loadReal();
        static std::string maskId(const std::string& id);

};

#endif