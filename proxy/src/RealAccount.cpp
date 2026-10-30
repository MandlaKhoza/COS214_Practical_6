#include <map>

#include "RealAccount.h"
#include "Account.h"

RealAccount::RealAccount(const Account* a, std::vector<std::string>& n) : account(a), internalNotes(n)
{

}

std::string RealAccount::display() const
{
    if (account == nullptr)
    {
        return "Account unavailable";
    }

    std::string result;
    result = account.describe();

    std::map<std::string, std::string> details;
}

std::string RealAccount::getAccountNumber() const
{
    return account->getAccountNumber();
}

const std::string& RealAccount::getOwnerId() const
{
    return account->getOwnerId();
}

std::string RealAccount::displaySummary() const
{

}

const std::vector<std::string>& RealAccount::getInternalNotes() const
{
    return internalNotes;
}