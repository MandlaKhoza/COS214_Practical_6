#include "GeneralAccountFactory.h"

#include "WorkflowInstance.h"
#include "Document.h"
#include "Timestamp.h"

#include "StudentAccount.h"
#include "EmployeeAccount.h"
#include "SurveyReceipt.h"


/**
 * @brief Constructs a GeneralAccountFactory object.
 *
 * Initialises the factory with an account number prefix and the
 * document field used to identify the owner of an account.
 *
 * @param prefix The prefix used when generating account numbers.
 * @param ownerField The name of the document field containing the account owner's ID.
 */
GeneralAccountFactory::GeneralAccountFactory(std::string prefix, std::string ownerField)
    : AccountFactory(prefix, ownerField){}



/**
 * @brief Creates an account based on the workflow type.
 *
 * Retrieves the workflow's document and extracts the information
 * required to construct the appropriate account. The factory supports
 * StudentAccount, EmployeeAccount and SurveyReceipt objects.
 *
 * A unique account number and the current creation timestamp are
 * generated before the account is constructed.
 *
 * @param wi The workflow instance containing the document with the account information.
 *
 * @return A unique pointer to the newly created Account object, or nullptr
 *         if the workflow type is unsupported or required document information is missing.
 */

std::unique_ptr<Account> GeneralAccountFactory::createAccount(WorkflowInstance& wi){
    const Document& document = wi.getDocument();

    const std::string& workflowType = document.getType();

    std::string accountNumber = prefix + nextAccountNumber();
    std::string ownerId = getOwnerId(wi);
    std::string createdAt = getCurrentTimestamp();

    if(workflowType == "StudentApplication"){
        std::string studentNumber;
        std::string faculty;
        std::string programme;

        if(!document.getField("studentNumber", studentNumber)){
            return nullptr;
        }

        if(!document.getField("faculty", faculty)){
            return nullptr;
        }

        if(!document.getField("programme", programme)){
            return nullptr;
        }

        return std::unique_ptr<Account>(
            new StudentAccount(accountNumber, workflowType, ownerId, createdAt,
                               studentNumber, faculty, programme)
        );
    }

    else if(workflowType == "EmployeeApplication"){
        std::string employeeNumber;
        std::string department;
        std::string systemAccess;

        if(!document.getField("employeeNumber", employeeNumber)){
            return nullptr;
        }

        if(!document.getField("department", department)){
            return nullptr;
        }

        if(!document.getField("systemAccess", systemAccess)){
            return nullptr;
        }

        return std::unique_ptr<Account>(
            new EmployeeAccount(accountNumber, workflowType, ownerId, createdAt,
                                employeeNumber, department, systemAccess)
        );
    }

    else if(workflowType == "Survey"){
        std::string responseId;
        double summaryScore;

        if(!document.getField("responseId", responseId)){
            return nullptr;
        }
        if(!document.getNumber("summaryScore", summaryScore)){
            return nullptr;
        }

        return std::unique_ptr<Account>(
            new SurveyReceipt(accountNumber, workflowType, ownerId, createdAt,
                              responseId, summaryScore)
        );
    }

    return nullptr;
}