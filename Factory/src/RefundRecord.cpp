#include "RefundRecord.h"
#include <sstream>

/**
 * @brief Constructs a RefundRecord object and initialises its attributes.
 *
 * @param number The unique account number.
 * @param type The type of workflow associated with the account.
 * @param ownerId The ID of the account owner.
 * @param createdAt The date and time the account was created.
 * @param orderNumber The order number associated with the refund.
 * @param amount The amount being refunded.
 * @param payoutMethod The method used to pay out the refund.
 */
RefundRecord::RefundRecord(std::string number, std::string type, std::string ownerId, std::string createdAt,
     std::string orderNumber, double amount, std::string payoutMethod)

    : Account(number, type, ownerId, createdAt), orderNumber(orderNumber), amount(amount),
      payoutMethod(payoutMethod){}


/**
 * @brief Build a string that describes all the details concerning a refund record using
 *        the stringstream library
 *
 * @return the string describing the account
 */
std::string RefundRecord::describe() const{
    std::stringstream ss;
    ss << Account::describe();
    ss << "Order Number: " << orderNumber << "\n";
    ss << "Refund Amount: " << amount << "\n";
    ss << "Payout Method: " << payoutMethod << "\n";
    return ss.str();
}


/**
 * @brief Retrieves all the details associated with this refund record.
 *
 * Stores both the common account attributes inherited from Account and
 * the refund specific attributes as key-value pairs.
 *
 * @return A map containing the account details, where each key identifies an attribute
 *         and its corresponding value stores that attribute's data.
 */
std::map<std::string, std::string> RefundRecord::getDetails() const{

    std::map<std::string, std::string> details;
    details["Account Number"] = accountNumber;
    details["Owner ID"] = ownerId;
    details["Created At"] = createdAt;
    details["Workflow Type"] = workflowType;
    details["Order Number"] = orderNumber;

    std::stringstream ss;
    ss << amount;
    details["Refund Amount"] = ss.str();
    details["Payout Method"] = payoutMethod;

    return details;
}