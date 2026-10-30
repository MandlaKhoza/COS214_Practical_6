#include "EmployeeAccount.h"
#include <sstream>

/**
 * @brief Constructs an EmployeeAccount object and initialises its attributes.
 *
 * @param number The unique account number.
 * @param type The type of workflow associated with the account.
 * @param ownerId The ID of the account owner.
 * @param createdAt The date and time the account was created.
 * @param employeeNumber The employee's employee number.
 * @param department The department the employee belongs to.
 * @param systemAccess The system access assigned to the employee.
 */
EmployeeAccount::EmployeeAccount(std::string number, std::string type, std::string ownerId, std::string createdAt,
     std::string employeeNumber, std::string department, std::string systemAccess)

    : Account(number, type, ownerId, createdAt), employeeNumber(employeeNumber), department(department),
      systemAccess(systemAccess){}


/**
 * @brief Build a string that describes all the details concerning an employee account using
 *        the stringstream library
 *
 * @return the string describing the account
 */
std::string EmployeeAccount::describe() const{
    std::stringstream ss;
    ss << Account::describe();
    ss << "Employee Number: " << employeeNumber << "\n";
    ss << "Department: " << department << "\n";
    ss << "System Access: " << systemAccess << "\n";
    return ss.str();
}


/**
 * @brief Retrieves all the details associated with this employee account.
 *
 * Stores both the common account attributes inherited from Account and
 * the employee specific attributes as key-value pairs.
 *
 * @return A map containing the account details, where each key identifies an attribute
 *         and its corresponding value stores that attribute's data.
 */
std::map<std::string, std::string> EmployeeAccount::getDetails() const{

    std::map<std::string, std::string> details;
    details["Account Number"] = accountNumber;
    details["Owner ID"] = ownerId;
    details["Created At"] = createdAt;
    details["Workflow Type"] = workflowType;
    details["Employee Number"] = employeeNumber;
    details["Department"] = department;
    details["System Access"] = systemAccess;

    return details;
}