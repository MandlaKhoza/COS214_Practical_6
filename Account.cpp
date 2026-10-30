#include "Account.h"
#include <iostream>
#include <sstream>


/**
 * @brief Constructs an Account object and initialises its attributes.
 *
 * @param number The unique account number.
 * @param type The type of workflow associated with the account.
 * @param ownerId The ID number of the account owner.
 * @param createdAt The date and time the account was created.
 */
Account::Account(std::string number, std::string type, std::string ownerId, std::string createdAt)
: accountNumber(number), workflowType(type), ownerId(ownerId), createdAt(createdAt) {}

/**
 * @brief Retrieves the account number.
 *
 * @return The unique account number.
 */
std::string Account::getAccountNumber() const{
    return accountNumber;
}

/**
 * @brief Retrieves the ID of the account owner.
 *
 * @return The account owner's ID number.
 */

std::string Account::getOwnerId() const{
    return ownerId;
}

/**
 * @brief Retrieves the account creation timestamp.
 *
 * @return The date and time the account was created.
 */
std::string Account::getCreatedAt() const{
    return createdAt;
}

/**
 * @brief Generates a string describing the account's details.
 *
 * Uses a stringstream to combine the common account attributes into
 * a formatted string. Derived account classes can extend this description by adding in thei
 * own fluff
 *
 * @return A string containing the account number, owner ID,
 *         creation timestamp and workflow type.
 */
std::string Account::describe() const{
    std::stringstream ss;
    ss << "Tividy Account Number: " << accountNumber << "\n";
    ss << "Owner ID number: " << ownerId << "\n";
    ss << "Created at: " << createdAt << "\n";
    ss << "Type of workflow: " << workflowType << "\n";
    return ss.str();
}
