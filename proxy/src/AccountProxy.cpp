#include "AccountProxy.h"
#include "Account.h"

AccountProxy::AccountProxy(const std::shared_ptr<AccountRegistry>& r, const std::string& aN, const Requester& req, 
    const std::shared_ptr<AccessLog>& l) : registry(r), accountNumber(aN), requester(req), log(l), real(nullptr)
{

}

AccessLevel AccountProxy::determineAccess() const
{
    Account a = registry->find(accountNumber);

    if(a == nullptr)
    {
        return AccessLevel::Denied;
    }
    else if(a.getUserRole() == Staff)
    {
        return AccessLevel::FullWithNotes;
    }
    else if(a.getUserRole() == Auditor)
    {
        return AccessLevel::Summary;
    }

    if ((a.getUserId() != nullptr)&&(a.getUserId == accountNumber.getUserId()))
    {
        return AccessLevel::Full;
    }

    return AccessLevel::Denied;
}

RealAccount& AccountProxy::loadReal() const
{
    if (real == nullptr)
    {

    }
}

std::string AccountProxy::display() const 
{

}

static std::string AccountProxy::maskId(const std::string& id)
{

}

std::string getAccountNumber() const
{
    return accountNumber;
}