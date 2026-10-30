#include "FinanceAccountFactory.h"

#include "WorkflowInstance.h"
#include "Document.h"

#include "LoanAccount.h"
#include "RefundRecord.h"

/**
 * @brief Constructs a FinanceAccountFactory object.
 *
 * Initialises the parent AccountFactory with the account number prefix
 * and the document field used to retrieve the account owner's ID.
 *
 * @param prefix The prefix used when generating account numbers.
 * @param ownerField The document field containing the account owner's ID.
 */
FinanceAccountFactory::FinanceAccountFactory(std::string prefix, std::string ownerField)
    : AccountFactory(prefix, ownerField){}


/**
 * @brief Creates the appropriate finance account based on the workflow type.
 *
 * Retrieves the workflow type and required account information from the
 * workflow's document, then creates either a LoanAccount or RefundRecord.
 *
 * @param w The workflow instance for which the account is being created.
 * @return A unique pointer to the created account, or nullptr if the
 *         workflow type is unsupported or required information is missing.
 */
std::unique_ptr<Account> FinanceAccountFactory::createAccount(WorkflowInstance& w){

    const Document& document = w.getDocument();

    const std::string& workflowType = document.getType();

    std::string accountNumber = prefix + nextAccountNumber();
    std::string ownerId = getOwnerId(w);

    if(workflowType == "LoanApplication"){

        double principal;
        double interestRate;
        int termMonths;
        double monthlyInstalment;
        double outstanding;

        if(!document.getNumber("principal", principal)){    
            return nullptr;
        }

        if(!document.getNumber("interestRate", interestRate)){
            return nullptr;
        }

        if(!document.getNumber("termMonths", principal)){
            return nullptr;
        }

        if(!document.getNumber("monthlyInstalment", monthlyInstalment)){
            return nullptr;
        }

        if(!document.getNumber("outstanding", outstanding)){
            return nullptr;
        }

        std::string createdAt = getCurrentTimestamp();
        return std::unique_ptr<Account>(new LoanAccount(accountNumber, workflowType,ownerId, createdAt, principal,
                                            interestRate, termMonths, monthlyInstalment, outstanding));
    }

    else if(workflowType == "Refund"){
        std::string orderNumber;
        double amount;
        std::string payoutMethod;

        if(!document.getField("orderNumber", orderNumber)){
            return nullptr;
        }

        if(!document.getNumber("amount", amount)){
            return nullptr;
        }

        if(!document.getField("payoutMethod", payoutMethod)){
            return nullptr;
        }
        std::string createdAt = getCurrentTimestamp();
        return std::unique_ptr<Account>(new RefundRecord(accountNumber, workflowType, ownerId, createdAt, orderNumber, amount, payoutMethod));
    }

    return nullptr;
}