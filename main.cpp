
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

static std::vector<std::shared_ptr<Comparable> > admissionRules() {
    std::vector<std::shared_ptr<Comparable> > rules;
    rules.push_back(std::shared_ptr<Comparable>(
        new Comparable(FieldNames::APS, 30.0, ">=")
    ));
    return rules;
}

static std::unique_ptr<WorkflowNode> makeUniversityChain(
    std::shared_ptr<ExternalCheck> qualificationCheck,
    std::shared_ptr<ApprovalChain> approvals)
{
    std::unique_ptr<WorkflowNode> completion(
        new DocCompletionCheckNode("ApplicationCompletion")
    );
    std::unique_ptr<WorkflowNode> eligibility(
        new RangeCheckNode("MinimumAPS", admissionRules())
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
    commonFields.push_back(FieldNames::ID_COPY);
    commonFields.push_back(FieldNames::CERTIFIED_RESULTS);

    std::vector<std::shared_ptr<Comparable> > comparableFields;
    comparableFields.push_back(std::shared_ptr<Comparable>(
        new Comparable(FieldNames::APS, 0.0, ">=")
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
            [](const ApprovalRequest& request) {
                double aps = 0.0;

                const bool accepted =
                    request.instance &&
                    request.instance->getDocument() &&
                    request.instance->getDocument()->getNumber(
                        FieldNames::APS, aps) &&
                    aps >= 35.0;

                return accepted
                    ? ApprovalDecision::Approved
                    : ApprovalDecision::Rejected;
            }
        )
    ));

    tui.run([&] {
        director.construct();

        WorkflowInstance workflow(
            "UNI-2026-001", "UniversityApplication", document
        );
        workflow.setChain(makeUniversityChain(qualificationCheck, approvals));

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

        if (workflow.isFinished() &&
            workflow.getRejectedAt().empty() &&
            observer->notificationCount() == 1 &&
            observer->received(Outcome::Approved)) {
            UI_LOG("[PASS] Application approved and observer notified exactly once");
        } else if (workflow.getRejectedAt().empty()) {
            UI_LOG("[WF] Workflow finished without a rejection node");
        } else {
            UI_LOG("[FAIL] Workflow rejected at " + workflow.getRejectedAt());
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
