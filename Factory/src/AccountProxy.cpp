#include "AccountProxy.h"
#include "Account.h"
#include "AccountRegistry.h"



AccountProxy::AccountProxy(const std::shared_ptr<AccountRegistry>& r, const std::string& aN, const Requester& req, 
    const std::shared_ptr<AccessLog>& l) : registry(r), accountNumber(aN), requester(req), log(l), real(nullptr)
{

}

AccessLevel AccountProxy::determineAccess() const
{
    const Account* a = registry->find(accountNumber);

    if(a == nullptr)
    {
        return AccessLevel::Denied;
    }
    else if(requester.role == UserRole::Staff)
    {
        return AccessLevel::FullWithNotes;
    }
    else if(requester.role == UserRole::Auditor)
    {
        return AccessLevel::Summary;
    }

    if ((!requester.userId.empty())&&(a->getOwnerId() == requester.userId))
    {
        return AccessLevel::Full;
    }

    return AccessLevel::Denied;
}

RealAccount& AccountProxy::loadReal() const
{
    if (!real)
    {
        const Account* a = registry->find(accountNumber);

        real.reset(new RealAccount(a, std::vector<std::string>()));
    }

    return *real;
}

std::string AccountProxy::display() const 
{
    AccessLevel level = determineAccess();
    log->record(requester, accountNumber, level);   

    switch (level)
    {
        case AccessLevel::Full:
            return loadReal().display();

        case AccessLevel::FullWithNotes:
        {
            RealAccount& r = loadReal();
            std::string result = r.display() + "\n\nInternal notes:";
            const std::vector<std::string>& notes = r.getInternalNotes();
            for (std::size_t i = 0; i < notes.size(); i++)
            {
                result += "\n- " + notes[i];
            }
            return result;
        }

        case AccessLevel::Summary:
        {
            RealAccount& r = loadReal();
            return r.displaySummary() + "\nOwner ID: " + maskId(r.getOwnerId());
        }

        case AccessLevel::Denied:
            break;
    }
    return "Access denied";
}

std::string AccountProxy::maskId(const std::string& id)
{
    if (id.size() <= 4)
    {
        return std::string(id.size(), '*');   
    }
    return std::string(id.size() - 4, '*') + id.substr(id.size() - 4);
}

std::string AccountProxy::getAccountNumber() const
{
    return accountNumber;
}