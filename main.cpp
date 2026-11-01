
#include <iomanip>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "DocumentBuilder/inc/Document.h"
#include "DocumentBuilder/inc/DocumentBuilder.h"
#include "DocumentBuilder/inc/Director.h"

#include "WorkflowInstance/inc/WorkflowInstance.h"
#include "WorkflowInstance/inc/WorkflowNode.h"
#include "WorkflowInstance/inc/DocCompletionCheckNode.h"
#include "WorkflowInstance/inc/RangeCheckNode.h"
#include "WorkflowInstance/inc/ExternalCheckNode.h"

#include "observer/inc/WorkflowObserver.h"

#include "Adapter/inc/ExamBoardAPI.h"
#include "Adapter/inc/ExamRecord.h"
#include "Adapter/inc/FieldNames.h"
#include "Adapter/inc/QualificationAdapter.h"
#include "Adapter/inc/ExternalCheck.h"

#include "CoR2/inc/ApprovalDecision.h"
#include "CoR2/inc/ApprovalNode.h"
#include "CoR2/inc/ApprovalChain.h"
#include "CoR2/inc/ApprovalRequest.h"
#include "CoR2/inc/RuleBasedApprover.h"

#include "Factory/inc/Account.h"
#include "Factory/inc/AccountFactory.h"
#include "Factory/inc/GeneralAccountFactory.h"
#include "Factory/inc/AccountRegistry.h"
#include "Factory/inc/AccountView.h"
#include "Factory/inc/AccountProxy.h"
#include "Factory/inc/AccessLog.h"
#include "Factory/inc/Requester.h"
#include "Factory/inc/UserRole.h"

#include "tui_app.hpp"
#include "ui_logger.hpp"

// Records notifications received from WorkflowInstance.
class RecordingObserver : public WorkflowObserver {
private:
    std::vector<Outcome> outcomes;

public:
    void onWorkflowFinished(WorkflowInstance&, Outcome outcome) override {
        outcomes.push_back(outcome);
        std::ostringstream oss;
        oss << "[WF] Observer notified with outcome " << static_cast<int>(outcome);
        UI_LOG(oss.str());
    }

    std::size_t notificationCount() const {
        return outcomes.size();
    }

    bool received(Outcome expected) const {
        for (std::size_t i = 0; i < outcomes.size(); ++i) {
            if (outcomes[i] == expected) {
                return true;
            }
        }

        return false;
    }
};

static void logLine(const std::string& label, const std::string& value) {
    std::ostringstream oss;
    oss << std::left << std::setw(23) << label << value;
    UI_LOG("[WF] " + oss.str());
}

static void logAccountView(const std::string& label, const AccountView& view) {
    UI_LOG("[PROXY] " + label);
    std::istringstream details(view.display());
    std::string line;
    while (std::getline(details, line)) {
        UI_LOG("[PROXY] " + line);
    }
}

static std::unique_ptr<WorkflowNode> makeUniversityChain(
    const std::vector<std::shared_ptr<Comparable> >& admissionRules,
    std::shared_ptr<ExternalCheck> qualificationCheck,
    std::shared_ptr<ApprovalChain> approvals)
{
    std::unique_ptr<WorkflowNode> completion(
        new DocCompletionCheckNode("ApplicationCompletion")
    );
    std::unique_ptr<WorkflowNode> eligibility(
        new RangeCheckNode("AdmissionAPS", admissionRules)
    );
    std::unique_ptr<WorkflowNode> qualifications(
        new ExternalCheckNode("VerifiedQualifications", qualificationCheck)
    );
    std::unique_ptr<WorkflowNode> faculty(
        new ApprovalNode("FacultyApproval", approvals)
    );

    qualifications->setNext(std::move(faculty));
    eligibility->setNext(std::move(qualifications));
    completion->setNext(std::move(eligibility));
    return completion;
}

int TUIApp::run() {
    UI_LOG("[TUI] TIVIDY Workflow Creation System");

    TUI tui;
    std::shared_ptr<Document> document(new Document());
    DocumentBuilder builder(document.get(), &tui);

    std::vector<std::string> commonFields;
    commonFields.push_back(FieldNames::NAME);
    commonFields.push_back(FieldNames::SURNAME);
    commonFields.push_back(FieldNames::EMAIL);
    commonFields.push_back(FieldNames::PROGRAMME);
    commonFields.push_back(FieldNames::EXAM_CANDIDATE_NUMBER);



    std::vector<std::shared_ptr<Comparable> > comparableFields;
    comparableFields.push_back(std::shared_ptr<Comparable>(
        new Comparable(FieldNames::APS, 35.0, ">=")
    ));
    std::vector<std::shared_ptr<Comparable> > admissionRules = comparableFields;
    admissionRules.push_back(std::shared_ptr<Comparable>(
        new Comparable(FieldNames::APS, 42.0, "<=")
    ));

    Director director(&builder, commonFields, comparableFields);
    director.prepare();

    std::shared_ptr<ExamBoardAPI> examBoard(new ExamBoardAPI());
    examBoard->addRecord(ExamRecord{
        "EXAM-2026-001",
        "Naledi Mokoena",
        {
            {"Mathematics", 75},
            {"English", 68}
        }
    });

    examBoard->addRecord(ExamRecord{
        "EXAM-2026-002",
        "Thabo Dlamini",
        {
            {"Mathematics", 55},
            {"English", 68}
        }
    });

    std::shared_ptr<ExternalCheck> qualificationCheck(
        new QualificationAdapter(
            examBoard,
            FieldNames::EXAM_CANDIDATE_NUMBER,
            {
                {"Mathematics", 60},
                {"English", 50}
            }
        )
    );

    std::shared_ptr<ApprovalChain> approvals(
        new ApprovalChain(ApprovalPolicy::SequentialSignOff)
    );
    approvals->setHead(std::unique_ptr<Approver>(
        new RuleBasedApprover(
            "Science Faculty",
            "Admissions Committee",
            [](const ApprovalRequest&) {
                // Earlier nodes enforce eligibility; faculty sign-off is the popup decision.
                return ApprovalDecision::Approved;
            }
        )
    ));

    std::shared_ptr<AccountFactory> accountFactory(
        new GeneralAccountFactory("STU-", FieldNames::EXAM_CANDIDATE_NUMBER)
    );
    std::shared_ptr<AccountRegistry> accountRegistry(new AccountRegistry());
    std::shared_ptr<AccessLog> accessLog(new AccessLog());
    unsigned int applicationCounter = 0;

    tui.run([&] {
        director.construct();
        const std::string applicationNumber = std::to_string(++applicationCounter);

        // Refresh university-assigned fields on every submission, including reruns.
        std::map<std::string, std::string> fields = document->getCommon();
        fields["studentNumber"] = "2026-" + applicationNumber;
        fields["faculty"] = "Science";
        document->setDocument(fields, document->getComparable());

        WorkflowInstance workflow(
            "UNI-2026-" + applicationNumber, "StudentApplication", document
        );
        workflow.setChain(makeUniversityChain(admissionRules, qualificationCheck, approvals));

        std::shared_ptr<RecordingObserver> observer(new RecordingObserver());
        workflow.attach(observer);

        logLine("Applicant fields:", "copied from Document Builder");
        logLine("Workflow:", workflow.getId());

        workflow.run();

        UI_LOG("[WF] NODE RESULTS");
        for (WorkflowNode* node = workflow.getChain();
             node;
             node = node->getNext()) {
            UI_LOG("[WF]   " + node->getName() + " -> " + node->getStateName());
        }

        if (workflow.isFailed()) {
            UI_LOG("[FAIL] Technical failure at " + workflow.getCurrent()->getName() +
                   "; the application was not rejected on admission criteria");
        } else if (!workflow.getRejectedAt().empty()) {
            UI_LOG("[FAIL] Workflow rejected at " + workflow.getRejectedAt());
        } else if (workflow.isCancelled()) {
            UI_LOG("[WF] Workflow cancelled");
        } else if (workflow.isFinished() &&
            observer->notificationCount() == 1 &&
            observer->received(Outcome::Approved)) {
            UI_LOG("[PASS] Application approved and observer notified exactly once");


            std::unique_ptr<Account> account = accountFactory->create(workflow);
            if (!account) {
                UI_LOG("[FAIL] Application approved, but the factory could not create a student account");
            } else {
                const std::string accountNumber = account->getAccountNumber();
                const std::string ownerId = account->getOwnerId();
                accountRegistry->add(account.release());
                UI_LOG("[FACTORY] Student account created: " + accountNumber);

                // The student attemps to view their own account and a rando request ot view their account
                AccountProxy studentView(accountRegistry, accountNumber,
                    Requester(ownerId, UserRole::Client), accessLog);
                logAccountView("Student viewing their own account:", studentView);

                AccountProxy unrelatedView(accountRegistry, accountNumber,
                    Requester(ownerId + "-other", UserRole::Client), accessLog);
                logAccountView("Unrelated applicant requesting the same account:", unrelatedView);

                UI_LOG("[PROXY] Total access attempts recorded: " +
                       std::to_string(accessLog->getAttempts().size()));
            }
        } else if (!workflow.isFinished()) {
            UI_LOG("[WF] Workflow has not finished; current node: " +
                   workflow.getCurrent()->getName());
        } else {
            UI_LOG("[FAIL] Workflow finished, but the expected observer notification was missing");
        }

        UI_LOG("[WF] APPLICATION HISTORY");
        const std::vector<HistoryEntry>& history = workflow.getHistory();
        for (std::size_t i = 0; i < history.size(); ++i) {
            UI_LOG("[WF]   " + history[i].nodeName + ": " + history[i].message);
        }
    });

    return 0;
}

int main() {
    return TUIApp::run();
}
