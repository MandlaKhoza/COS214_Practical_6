#include "GeneralAccountFactory.h"

#include "WorkflowInstance.h"
#include "Document.h"

#include "StudentAccount.h"
#include "EmployeeAccount.h"
#include "SurveyReceipt.h"

GeneralAccountFactory::GeneralAccountFactory(std::string prefix, std::string ownerField)
    : AccountFactory(prefix, ownerField){}


    
std::unique_ptr<Account> GeneralAccountFactory::createAccount(WorkflowInstance& wi){
    const Document& document = wi.getDocument();

    const std::string& workflowType = document.getType();

    std::string accountNumber = nextAccountNumber();
    std::string ownerId = getOwnerId(wi);

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
        return std::unique_ptr<Account>(new StudentAccount(accountNumber, workflowType, ownerId, studentNumber, faculty, programme));
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
        return std::unique_ptr<Account>(new EmployeeAccount(accountNumber, workflowType, ownerId, employeeNumber, department, systemAccess));
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
        return std::unique_ptr<Account>(new SurveyReceipt(accountNumber, workflowType, ownerId, responseId, summaryScore));
    }
    return nullptr;
}