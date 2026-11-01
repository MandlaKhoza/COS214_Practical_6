#include "LoanAccount.h"
#include <sstream>

/**
 * @brief Constructs a LoanAccount object and initialises its attributes.
 *
 * @param number The unique account number.
 * @param type The type of workflow associated with the account.
 * @param ownerId The ID of the account owner.
 * @param createdAt The date and time the account was created.
 * @param principal The original loan amount.
 * @param interestRate The interest rate applicable to the loan.
 * @param termMonths The loan repayment period in months.
 * @param monthlyInstalment The monthly repayment amount.
 * @param outstanding The remaining amount owed on the loan.
 */
LoanAccount::LoanAccount(std::string number, std::string type, std::string ownerId,
                        std::string createdAt, double principal, double interestRate,
                        int termMonths, double monthlyInstalment, double outstanding)

                        : Account(number, type, ownerId, createdAt), principal(principal), interestRate(interestRate),
                        termMonths(termMonths), monthlyInstalment(monthlyInstalment), outstanding(outstanding){}

/**
 * @brief Build a string that describes all the details concerning a loan account using
 *        the stringstream library
 *
 * @return the string describing the account
 */
std::string LoanAccount::describe() const{
    std::stringstream ss;
    ss << Account::describe();
    ss << "Principal: " << principal << "\n";
    ss << "Interest Rate: " << interestRate << "%\n";
    ss << "Term (in months): " << termMonths << "\n";
    ss << "Monthly Instalment: " << monthlyInstalment << "\n";
    ss << "Outstanding Balance: " << outstanding << "\n";
    return ss.str();
}

/**
 * @brief Retrieves all the details associated with this loan account.
 *
 * Stores both the common account attributes inherited from Account and
 * the loan specific attributes as key-value pairs.
 *
 * @return A map containing the account details, where each key identifies an attribute
 *         and its corresponding value stores that attribute's data.
 */
std::map<std::string, std::string> LoanAccount::getDetails() const{

    std::map<std::string, std::string> details;
    details["Account Number"] = accountNumber;
    details["Owner ID"] = ownerId;
    details["Created At"] = createdAt;
    details["Workflow Type"] = workflowType;

    std::stringstream ss;

    ss << principal;
    details["Principal"] = ss.str();

    ss.str("");
    ss << interestRate;
    details["Interest Rate"] = ss.str();

    ss.str("");
    ss << termMonths;
    details["Term (Months)"] = ss.str();

    ss.str("");
    ss << monthlyInstalment;
    details["Monthly Instalment"] = ss.str();

    ss.str("");
    ss << outstanding;
    details["Outstanding Balance"] = ss.str();

    return details;
}