#include "StudentAccount.h"
#include <sstream>
/**
 * @brief Constructs a StudentAccount object and initialises its attributes.
 *
 * @param number The unique account number.
 * @param type The type of workflow associated with the account.
 * @param ownerId The ID of the account owner.
 * @param createdAt The date and time the account was created.
 * @param studentNumber The student's student number.
 * @param faculty The faculty the student belongs to.
 * @param programme The programme the student is enrolled in.
 */
StudentAccount::StudentAccount(std::string number, std::string type, std::string ownerId, std::string createdAt,
     std::string studentNumber, std::string faculty, std::string programme)

    : Account(number, type, ownerId, createdAt), studentNumber(studentNumber), faculty(faculty),
      programme(programme){}



/**
 * @brief Build a string that describes all the details concerning an account using 
 *        the stringstream library
 * 
 * @return the string describing the account
 */
std::string StudentAccount::describe() const{
    std::stringstream ss;
    ss << Account::describe();
    ss << "Student Number: " << studentNumber << "\n";
    ss << "Faculty: " << faculty << "\n";
    ss << "Programme enrolled for: " << programme << "\n";
    return ss.str();
}


/**
 * @brief Retrieves all the details associated with this student account.
 *
 * Stores both the common account attributes inherited from Account and
 * the student specific attributes as  key-value pairs.
 *
 * @return A map containing the account details, where each key identifies an attribute
 *         and its corresponding value stores that attribute's data.
 */
std::map<std::string, std::string> StudentAccount::getDetails() const{

    std::map<std::string, std::string> details;
    details["Account Number"] = accountNumber;
    details["Owner ID"] = ownerId;
    details["Created At"] = createdAt;
    details["Workflow Type"] = workflowType;
    details["Student Number"] = studentNumber;
    details["Faculty"] = faculty;
    details["Programme"] = programme;

    return details;
}