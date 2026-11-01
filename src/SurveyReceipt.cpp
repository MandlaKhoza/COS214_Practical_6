#include "SurveyReceipt.h"
#include <sstream>

/**
 * @brief Constructs a SurveyReceipt object and initialises its attributes.
 *
 * @param number The unique account number.
 * @param type The type of workflow associated with the account.
 * @param ownerId The ID of the account owner.
 * @param createdAt The date and time the account was created.
 * @param responseId The unique ID associated with the survey response.
 * @param summaryScore The overall score obtained from the survey.
 */

SurveyReceipt::SurveyReceipt(std::string number, std::string type, std::string ownerId, std::string createdAt,
     std::string responseId, double summaryScore)
    : Account(number, type, ownerId, createdAt), responseId(responseId), summaryScore(summaryScore){}


/**
 * @brief Build a string that describes all the details concerning a survey receipt using
 *        the stringstream library
 *
 * @return the string describing the account
 */
std::string SurveyReceipt::describe() const{
    std::stringstream ss;
    ss << Account::describe();
    ss << "Response ID: " << responseId << "\n";
    ss << "Summary Score: " << summaryScore << "\n";
    return ss.str();
}


/**
 * @brief Retrieves all the details associated with this survey receipt.
 *
 * Stores both the common account attributes inherited from Account and
 * the survey specific attributes as key-value pairs.
 *
 * @return A map containing the account details, where each key identifies an attribute
 *         and its corresponding value stores that attribute's data.
 */
std::map<std::string, std::string> SurveyReceipt::getDetails() const{

    std::map<std::string, std::string> details;
    details["Account Number"] = accountNumber;
    details["Owner ID"] = ownerId;
    details["Created At"] = createdAt;
    details["Workflow Type"] = workflowType;
    details["Response ID"] = responseId;

    std::stringstream ss;
    ss << summaryScore;
    details["Summary Score"] = ss.str();

    return details;
}