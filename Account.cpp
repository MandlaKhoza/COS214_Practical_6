#include "Account.h"
#include <iostream>
#include <sstream>

Account::Account(std::string number, std::string type, std::string ownerId, std::string createdAt)
: accountNumber(number), workflowType(type), ownerId(ownerId), createdAt(createdAt) {}


std::string Account::getAccountNumber() const{
    return accountNumber;
}


std::string Account::getOwnerId() const{
    return ownerId;
}

std::string Account::getCreatedAt() const{
    return createdAt;
}

