#include <map>

#include "RealAccount.h"
#include "Account.h"

RealAccount::RealAccount(const Account* a, const std::vector<std::string>& n) : account(a), internalNotes(n)
{

}

std::string RealAccount::display() const
{
    if (account == nullptr)
    {
        return "Account unavailable";
    }

    std::string result;
    result = account->describe();

    std::map<std::string, std::string> details;
    details = account->getDetails();

    for (const auto& entry: details)
    {
        result += "\n" + entry.first + ": " + entry.second;
    }

    return result;
}

std::string RealAccount::getAccountNumber() const
{
    if (account == nullptr)
    {
        return "";
    }
    return account->getAccountNumber();
}

const std::string RealAccount::getOwnerId() const
{
    return account->getOwnerId();
}

std::string RealAccount::displaySummary() const
{
    if (account == nullptr)
    {
        return "Account unavailable";
    }

    std::string result;

    result = "Account number: " + account->getAccountNumber() +
        "\n Type: " + account->getWorkflowType() +
        "\n Created: " + account->getCreatedAt();

    return result;
}

const std::vector<std::string>& RealAccount::getInternalNotes() const
{
    return internalNotes;
}