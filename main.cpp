#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Facade/inc/WorkflowFacade.h"
#include "DocumentBuilder/inc/Document.h"
#include "DocumentBuilder/inc/Comparable.h"
#include "WorkflowInstance/inc/WorkflowInstance.h"
#include "WorkflowInstance/inc/WorkflowNode.h"
#include "WorkflowInstance/inc/DocCompletionCheckNode.h"
#include "WorkflowInstance/inc/RangeCheckNode.h"
#include "WorkflowInstance/inc/ExternalCheckNode.h"
#include "Adapter/inc/ExamBoardAPI.h"
#include "Adapter/inc/ExamRecord.h"
#include "Adapter/inc/ExternalCheck.h"
#include "Adapter/inc/FieldNames.h"
#include "Adapter/inc/QualificationAdapter.h"
#include "Adapter/inc/LegacyCreditBureau.h"
#include "Adapter/inc/CreditRecord.h"
#include "Adapter/inc/CreditCheckAdapter.h"
#include "CoR2/inc/ApprovalChain.h"
#include "CoR2/inc/ApprovalNode.h"
#include "CoR2/inc/ApprovalRequest.h"
#include "CoR2/inc/RuleBasedApprover.h"
#include "Factory/inc/Account.h"
#include "Factory/inc/AccountFactory.h"
#include "Factory/inc/GeneralAccountFactory.h"
#include "Factory/inc/FinanceAccountFactory.h"
#include "Factory/inc/StudentAccount.h"
#include "Factory/inc/LoanAccount.h"
#include "Factory/inc/AccountRegistry.h"
#include "Factory/inc/AccountProxy.h"
#include "Factory/inc/AccessLog.h"
#include "Factory/inc/Requester.h"
#include "Factory/inc/UserRole.h"
#include "Command/include/WorkflowController.h"
#include "Command/include/ResetWorkflowCommand.h"
#include "ui_logger.hpp"

namespace {

void reportCheck(bool passed, const std::string& message) {
    UI_LOG(std::string(passed ? "[PASS] " : "[FAIL] ") + message);
}

void logDetails(const std::string& details) {
    std::istringstream lines(details);
    std::string line;
    while (std::getline(lines, line)) {
        UI_LOG("[PROXY] " + line);
    }
}

// This is client policy: omit this call for a workflow that should not create accounts.
void enableAccountDemo(WorkflowFacade& system, std::shared_ptr<AccountFactory> factory) {
    std::shared_ptr<AccountRegistry> registry(new AccountRegistry());
    std::shared_ptr<AccessLog> accessLog(new AccessLog());

    system.onApproved([factory, registry, accessLog](WorkflowInstance& workflow) {
        std::unique_ptr<Account> account = factory->create(workflow);
        if (!account) {
            reportCheck(false, "Factory could not create an account for " + workflow.getType());
            return;
        }

        const bool correctType =
            (workflow.getType() == "StudentApplication" &&
             dynamic_cast<StudentAccount*>(account.get()) != nullptr) ||
            (workflow.getType() == "LoanApplication" &&
             dynamic_cast<LoanAccount*>(account.get()) != nullptr);
        reportCheck(correctType, "Factory produced the expected account type");

        const std::string accountNumber = account->getAccountNumber();
        const std::string ownerId = account->getOwnerId();
        registry->add(account.release());
        UI_LOG("[FACTORY] Account registered: " + accountNumber);

        const std::size_t attemptsBefore = accessLog->getAttempts().size();
        const std::size_t deniedBefore = accessLog->getDenied().size();
        AccountProxy ownerView(registry, accountNumber,
            Requester(ownerId, UserRole::Client), accessLog);
        AccountProxy unrelatedView(registry, accountNumber,
            Requester(ownerId + "-other", UserRole::Client), accessLog);
        const std::string ownerDetails = ownerView.display();
        const std::string unrelatedDetails = unrelatedView.display();
        logDetails("Owner view:\n" + ownerDetails);
        logDetails("Unrelated requester: " + unrelatedDetails);
        reportCheck(!ownerDetails.empty() && ownerDetails != "Access denied" &&
                    ownerDetails != "Account unavailable" && unrelatedDetails == "Access denied" &&
                    accessLog->getAttempts().size() == attemptsBefore + 2 &&
                    accessLog->getDenied().size() == deniedBefore + 1,
                    "Proxy grants owner access, denies unrelated access and logs both attempts");

        // Exercise Command and State without rerunning approval or creating a second account.
        WorkflowController controller;
        bool reset = controller.execute(std::unique_ptr<Command>(new ResetWorkflowCommand(&workflow)));
        for (WorkflowNode* node = workflow.getChain(); node; node = node->getNext()) {
            reset = reset && node->getStateName() == "Waiting";
        }
        reportCheck(reset && !workflow.isFinished() && controller.getAuditLog().size() == 1,
                    "Reset command returns all nodes to Waiting (created account is retained)");
    });
}

int runStudentScenario() {
    WorkflowFacade system("StudentApplication", "UNI-2026-",
        [](Document& document, const std::string& applicationNumber) {
            std::map<std::string, std::string> fields = document.getCommon();
            fields["studentNumber"] = "2026-" + applicationNumber;
            fields["faculty"] = "Science";
            document.setDocument(fields, document.getComparable());
        });
    std::shared_ptr<Comparable> minimumAPS(new Comparable(FieldNames::APS, 35.0, ">="));
    system.prepareDocument(
        {FieldNames::NAME, FieldNames::SURNAME, FieldNames::EMAIL,
         FieldNames::PROGRAMME, FieldNames::EXAM_CANDIDATE_NUMBER},
        {minimumAPS},
        {{FieldNames::NAME, "Naledi"}, {FieldNames::SURNAME, "Mokoena"},
         {FieldNames::EMAIL, "naledi@example.com"}, {FieldNames::PROGRAMME, "BSc Computer Science"},
         {FieldNames::EXAM_CANDIDATE_NUMBER, "EXAM-2026-001"}, {FieldNames::APS, "36"}}
    );
    std::vector<std::shared_ptr<Comparable> > admissionRules = {
        minimumAPS, std::shared_ptr<Comparable>(new Comparable(FieldNames::APS, 42.0, "<="))
    };

    std::shared_ptr<ExamBoardAPI> examBoard(new ExamBoardAPI());
    examBoard->addRecord(ExamRecord{
        "EXAM-2026-001", "Naledi Mokoena", {{"Mathematics", 75}, {"English", 68}}
    });
    examBoard->addRecord(ExamRecord{
        "EXAM-2026-002", "Thabo Dlamini", {{"Mathematics", 55}, {"English", 68}}
    });
    std::shared_ptr<ExternalCheck> qualificationCheck(new QualificationAdapter(
        examBoard, FieldNames::EXAM_CANDIDATE_NUMBER, {{"Mathematics", 60}, {"English", 50}}
    ));
    std::shared_ptr<ApprovalChain> approvals(new ApprovalChain(ApprovalPolicy::SequentialSignOff));
    approvals->setHead(std::unique_ptr<Approver>(new RuleBasedApprover(
        "Science Faculty", "Admissions Committee",
        [](const ApprovalRequest&) {
            // The popup supplies the faculty decision.
            return ApprovalDecision::Approved;
        }
    )));

    system.addNode(std::unique_ptr<WorkflowNode>(new DocCompletionCheckNode("ApplicationCompletion")));
    system.addNode(std::unique_ptr<WorkflowNode>(new RangeCheckNode("AdmissionAPS", admissionRules)));
    system.addNode(std::unique_ptr<WorkflowNode>(new ExternalCheckNode("VerifiedQualifications", qualificationCheck)));
    system.addNode(std::unique_ptr<WorkflowNode>(new ApprovalNode("FacultyApproval", approvals)));
    enableAccountDemo(system, std::shared_ptr<AccountFactory>(
        new GeneralAccountFactory("STU-", FieldNames::EXAM_CANDIDATE_NUMBER)));
    return system.run();
}

int runLoanScenario() {
    WorkflowFacade system("LoanApplication", "LOAN-2026-",
        [](Document& document, const std::string&) {
            const double missing = std::numeric_limits<double>::quiet_NaN();
            double principal = missing;
            double term = missing;
            document.getNumber(FieldNames::LOAN_AMOUNT, principal);
            document.getNumber(FieldNames::TERM_MONTHS, term);
            std::map<std::string, double> numbers = document.getComparable();
            numbers["principal"] = principal;
            numbers["interestRate"] = 0.12;
            numbers["outstanding"] = principal;
            // Simple-interest demo: annual rate 12%, spread over the requested term.
            numbers["monthlyInstalment"] = std::isfinite(principal) && std::isfinite(term) && term > 0
                ? principal * (1.0 + 0.12 * term / 12.0) / term : missing;
            document.setDocument(document.getCommon(), numbers);
        });
    std::vector<std::shared_ptr<Comparable> > loanFields = {
        std::shared_ptr<Comparable>(new Comparable(FieldNames::LOAN_AMOUNT, 1000.0, ">=")),
        std::shared_ptr<Comparable>(new Comparable(FieldNames::TERM_MONTHS, 1.0, ">=")),
        std::shared_ptr<Comparable>(new Comparable(FieldNames::MONTHLY_INCOME, 5000.0, ">=")),
        std::shared_ptr<Comparable>(new Comparable(FieldNames::MONTHLY_EXPENSES, 0.0, ">="))
    };
    system.prepareDocument(
        {FieldNames::NAME, FieldNames::SURNAME, FieldNames::EMAIL, FieldNames::ID_NUMBER},
        loanFields,
        {{FieldNames::NAME, "Sam"}, {FieldNames::SURNAME, "Nkosi"},
         {FieldNames::EMAIL, "sam@example.com"}, {FieldNames::ID_NUMBER, "LOAN-001"},
         {FieldNames::LOAN_AMOUNT, "12000"}, {FieldNames::TERM_MONTHS, "12"},
         {FieldNames::MONTHLY_INCOME, "10000"}, {FieldNames::MONTHLY_EXPENSES, "4000"}}
    );
    std::vector<std::shared_ptr<Comparable> > loanRules = loanFields;
    loanRules.push_back(std::shared_ptr<Comparable>(new Comparable(FieldNames::LOAN_AMOUNT, 100000.0, "<=")));
    loanRules.push_back(std::shared_ptr<Comparable>(new Comparable(FieldNames::TERM_MONTHS, 60.0, "<=")));

    std::shared_ptr<LegacyCreditBureau> bureau(new LegacyCreditBureau());
    bureau->addRecord(CreditRecord{"LOAN-001", 720, 2000.0, false});
    bureau->addRecord(CreditRecord{"LOAN-002", 580, 10000.0, true});
    std::shared_ptr<ExternalCheck> creditCheck(new CreditCheckAdapter(bureau, FieldNames::ID_NUMBER, 650));
    std::shared_ptr<ApprovalChain> approvals(new ApprovalChain(ApprovalPolicy::SequentialSignOff));
    approvals->setHead(std::unique_ptr<Approver>(new RuleBasedApprover(
        "Branch Manager", "Lending",
        [](const ApprovalRequest& request) {
            if (!request.instance || !request.instance->getDocument()) {
                return ApprovalDecision::Rejected;
            }
            std::shared_ptr<Document> document = request.instance->getDocument();
            double income = 0.0, expenses = 0.0, instalment = 0.0, term = 0.0;
            const bool affordable =
                document->getNumber(FieldNames::MONTHLY_INCOME, income) &&
                document->getNumber(FieldNames::MONTHLY_EXPENSES, expenses) &&
                document->getNumber("monthlyInstalment", instalment) &&
                document->getNumber(FieldNames::TERM_MONTHS, term) &&
                std::floor(term) == term && instalment <= (income - expenses) * 0.30;
            return affordable ? ApprovalDecision::Approved : ApprovalDecision::Rejected;
        }
    )));

    // Different nodes, adapter, rules and order, configured entirely by this client.
    system.addNode(std::unique_ptr<WorkflowNode>(new DocCompletionCheckNode("LoanDocumentCompletion")));
    system.addNode(std::unique_ptr<WorkflowNode>(new ExternalCheckNode("CreditHistory", creditCheck)));
    system.addNode(std::unique_ptr<WorkflowNode>(new RangeCheckNode("LoanTerms", loanRules)));
    system.addNode(std::unique_ptr<WorkflowNode>(new ApprovalNode("LoanApproval", approvals)));
    enableAccountDemo(system, std::shared_ptr<AccountFactory>(
        new FinanceAccountFactory("LOAN-", FieldNames::ID_NUMBER)));
    return system.run();
}

}

int TUIApp::run() {
    const int studentResult = runStudentScenario();
    if (studentResult != 0) {
        return studentResult;
    }
    UILogger::instance().clear();
    return runLoanScenario();
}

int main() {
    return TUIApp::run();
}
