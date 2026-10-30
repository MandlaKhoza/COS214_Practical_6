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


std::string Account::describe() const{
    std::stringstream ss;
    ss << "Tividy Account Number: " << accountNumber << "\n";
    ss << "Owner ID number: " << ownerId << "\n";
    ss << "Created at: " << createdAt << "\n";
    ss << "Type of workflow: " << workflowType << "\n";
    return ss.str();
}
