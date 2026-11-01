
#include <cmath>
#include <iostream>
#include <iomanip>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

#include "DocumentBuilder/inc/Document.h"

#include "WorkflowInstance/inc/WorkflowInstance.h"
#include "WorkflowInstance/inc/WorkflowNode.h"
#include "WorkflowInstance/inc/WorkflowDefinition.h"
#include "WorkflowInstance/inc/DocCompletionCheckNode.h"
#include "WorkflowInstance/inc/RangeCheckNode.h"
#include "WorkflowInstance/inc/NodeResult.h"

#include "Command/include/CancelWorkflowCommand.h"
#include "Command/include/ResetWorkflowCommand.h"

#include "observer/inc/WorkflowObserver.h"

#include "WorkflowInstance/inc/ExternalCheckNode.h"

#include "Adapter/inc/ExamBoardAPI.h"
#include "Adapter/inc/ExamRecord.h"
#include "Adapter/inc/FieldNames.h"
#include "Adapter/inc/QualificationAdapter.h"

#include "CoR2/inc/ApprovalNode.h"
#include "CoR2/inc/ApprovalChain.h"
#include "CoR2/inc/ApprovalRequest.h"
#include "CoR2/inc/RuleBasedApprover.h"


// ============================================================
// TEST FRAMEWORK
// ============================================================

namespace Test {

    int passed = 0;
    int failed = 0;
    int total = 0;

    void heading(const std::string& title) {
        std::cout << "\n";
        std::cout << "------------------------------------------------------------\n";
        std::cout << "  " << title << "\n";
        std::cout << "------------------------------------------------------------\n";
    }

    void check(const std::string& description, bool condition) {
        ++total;

        if (condition) {
            ++passed;
            std::cout << "  [PASS] " << description << "\n";
        } else {
            ++failed;
            std::cout << "  [FAIL] " << description << "\n";
        }
    }

    void detail(const std::string& message) {
        std::cout << "         " << message << "\n";
    }

    void workflowLine(const std::string& label,
                      const std::string& value) {
        std::cout << "  " << std::left << std::setw(23)
                  << label << value << "\n";
    }

    void nodeStates(WorkflowNode* node) {
        while (node) {
            std::cout << "         " << std::left
                      << std::setw(25) << node->getName()
                      << node->getStateName() << "\n";

            node = node->getNext();
        }
    }

    void summary() {
        std::cout << "\n";
        std::cout << "============================================================\n";
        std::cout << "                    TEST SUMMARY\n";
        std::cout << "============================================================\n";

        std::cout << "  Total checks : " << total << "\n";
        std::cout << "  Passed       : " << passed << "\n";
        std::cout << "  Failed       : " << failed << "\n";

        std::cout << "------------------------------------------------------------\n";

        if (failed == 0) {
            std::cout << "  RESULT: ALL CHECKS PASSED\n";
        } else {
            std::cout << "  RESULT: SOME CHECKS FAILED\n";
        }

        std::cout << "============================================================\n";
    }
}

// ============================================================
// TEST SUPPORT CLASSES
// ============================================================




// A node that requests human approval.
class ApprovalTestNode : public WorkflowNode {
protected:
    NodeResult process(WorkflowInstance&) {
        NodeResult result;
        result.result = PENDING;
        result.name = "Awaiting human approval";
        return result;
    }

public:
    ApprovalTestNode(const std::string& name)
        : WorkflowNode(name) {}

    std::unique_ptr<WorkflowNode> clone() {
        return std::unique_ptr<WorkflowNode>(
            new ApprovalTestNode(getName()));
    }
};

// A node that deliberately throws an exception.
class FailingTestNode : public WorkflowNode {
protected:
    NodeResult process(WorkflowInstance&) {
        throw std::runtime_error("Simulated processing failure");
    }

public:
    FailingTestNode(const std::string& name)
        : WorkflowNode(name) {}

    std::unique_ptr<WorkflowNode> clone() {
        return std::unique_ptr<WorkflowNode>(
            new FailingTestNode(getName()));
    }
};

// Records notifications received from WorkflowInstance.
class RecordingObserver : public WorkflowObserver {
private:
    std::vector<Outcome> outcomes;

public:
    void onWorkflowFinished(WorkflowInstance&, Outcome outcome) {
        outcomes.push_back(outcome);
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

// ============================================================
// DOCUMENT FACTORIES
// ============================================================

std::shared_ptr<Document> makeDocument(
    std::map<std::string, std::string> common,
    std::map<std::string, double> comparable)
{
    std::shared_ptr<Document> document(new Document());

    document->setDocument(
        std::move(common),
        std::move(comparable));

    return document;
}

std::shared_ptr<Document> validDocument() {
    return makeDocument(
        {
            {"name", "John Doe"},
            {"address", "123 Main Street"},
            {"email", "john@example.com"}
        },
        {
            {"age", 30.0},
            {"salary", 50000.0},
            {"score", 85.0}
        }
    );
}

std::vector<std::shared_ptr<Comparable> > standardRules() {
    std::vector<std::shared_ptr<Comparable> > rules;

    rules.push_back(
        std::shared_ptr<Comparable>(
            new Comparable("age", 18.0, ">=")));

    rules.push_back(
        std::shared_ptr<Comparable>(
            new Comparable("salary", 30000.0, ">=")));

    rules.push_back(
        std::shared_ptr<Comparable>(
            new Comparable("score", 70.0, ">=")));

    return rules;
}

std::unique_ptr<WorkflowNode> makeStandardChain() {
    std::unique_ptr<DocCompletionCheckNode> head(
        new DocCompletionCheckNode("DocumentVerification"));

    std::unique_ptr<RangeCheckNode> range(
        new RangeCheckNode("EligibilityCheck", standardRules()));

    head->setNext(std::move(range));

    return std::unique_ptr<WorkflowNode>(head.release());
}
void runUniversityApplicationDemo() {
    Test::heading("16. UNIVERSITY APPLICATION SCENARIO");

    // Fictional programme requirements for this demonstration.
    std::shared_ptr<Document> application = makeDocument(
        {
            {FieldNames::NAME, "Naledi"},
            {FieldNames::SURNAME, "Mokoena"},
            {FieldNames::EMAIL, "naledi.mokoena@example.com"},
            {FieldNames::PROGRAMME, "BSc Computer Science"},
            {FieldNames::EXAM_CANDIDATE_NUMBER, "EXAM-2026-001"},
            {FieldNames::ID_COPY, "certified-id.pdf"},
            {FieldNames::CERTIFIED_RESULTS, "matric-results.pdf"}
        },
        {
            {FieldNames::APS, 36.0}
        }
    );

    // ExamBoard(ExamBoardAPI) holds ExamRecord
    std::shared_ptr<ExamBoardAPI> examBoard(new ExamBoardAPI());
    examBoard->addRecord(ExamRecord{
        "EXAM-2026-001",
        "Naledi Mokoena",
        {
            {"Mathematics", 75},
            {"English", 68}
        }
    });

    // qualificationCheck -> QualtificationAdapter holds examBoard and requiredMark
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

    // Requires APS >= 30. Comparable for APS
    std::vector<std::shared_ptr<Comparable> > admissionRules;
    admissionRules.push_back(std::shared_ptr<Comparable>(
        new Comparable(FieldNames::APS, 30.0, ">=")
    ));

    // The faculty applies a stricter selection requirement: APS >= 35
    // Human Approval Chain
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

    //////  Workflow Node creation  //////
    // COMPLETIONCHECK 
    std::unique_ptr<WorkflowNode> completion(
        new DocCompletionCheckNode("ApplicationCompletion")
    );
    // RANGECHECK
    std::unique_ptr<WorkflowNode> eligibility(
        new RangeCheckNode("MinimumAPS", admissionRules)
    );
    // EXTERNAL QUALIFICATION CHECK
    std::unique_ptr<WorkflowNode> qualifications(
        new ExternalCheckNode("VerifiedQualifications", qualificationCheck)
    );
    // HUMAN APPROVAL
    std::unique_ptr<WorkflowNode> faculty(
        new ApprovalNode("FacultyApproval", approvals)
    );

    // Assemble from the tail because each node owns its successor.
    qualifications->setNext(std::move(faculty));
    eligibility->setNext(std::move(qualifications));
    completion->setNext(std::move(eligibility));

    WorkflowInstance workflow(
        "UNI-2026-001", "UniversityApplication", application
    );
    workflow.setChain(std::move(completion));

    // OBSERVER
    std::shared_ptr<RecordingObserver> observer(new RecordingObserver());
    workflow.attach(observer);

    Test::workflowLine("Applicant:", "Naledi Mokoena");
    Test::workflowLine("Programme:", "BSc Computer Science");
    Test::workflowLine("Workflow:", workflow.getId());

    workflow.run();

    std::cout << "\n  NODE RESULTS\n";
    Test::nodeStates(workflow.getChain()); // Uses Test nodeStates printer

    for (WorkflowNode* node = workflow.getChain();
         node;
         node = node->getNext()) {
        Test::check(
            node->getName() + " passed",
            node->getStateName() == "Passed"
        );
    }

    Test::check(
        "Application approved and observer notified exactly once",
        workflow.isFinished() &&
        workflow.getRejectedAt().empty() &&
        observer->notificationCount() == 1 &&
        observer->received(Outcome::Approved)
    );

    std::cout << "\n  APPLICATION HISTORY\n";
    for (const HistoryEntry& entry : workflow.getHistory()) {
        std::cout << "    " << entry.nodeName
                  << ": " << entry.message << "\n";
    }
}
// ============================================================
// MAIN
// ============================================================

int main() {

    std::cout << "\n";
    std::cout << "============================================================\n";
    std::cout << "                 TIVIDY WORKFLOW SYSTEM\n";
    std::cout << "                  INTEGRATION TESTS\n";
    std::cout << "============================================================\n";

    std::cout << "  Testing workflow execution, validation, state transitions,\n";
    std::cout << "  commands, observers, history, and cloning.\n";

    std::shared_ptr<Document> document = validDocument();

    // --------------------------------------------------------
    // 1. DOCUMENT CONSTRUCTION
    // --------------------------------------------------------

    Test::heading("1. DOCUMENT CONSTRUCTION");

    Test::workflowLine(
        "Common fields:",
        std::to_string(document->getCommon().size()));

    Test::workflowLine(
        "Comparable fields:",
        std::to_string(document->getComparable().size()));

    Test::check(
        "Document contains the expected common fields",
        document->getCommon().size() == 3);

    Test::check(
        "Document contains the expected comparable fields",
        document->getComparable().size() == 3);

    // --------------------------------------------------------
    // 2. SUCCESSFUL WORKFLOW
    // --------------------------------------------------------

    Test::heading("2. SUCCESSFUL WORKFLOW EXECUTION");

    {
        WorkflowInstance wi("WF-001", "Application", document);

        wi.setChain(makeStandardChain());

        std::shared_ptr<RecordingObserver> observer(
            new RecordingObserver());

        wi.attach(observer);

        std::cout << "\n  Applicant: John Doe\n";
        std::cout << "  Workflow : " << wi.getId() << "\n\n";

        wi.run();

        std::cout << "  EXECUTION RESULTS\n";
        Test::nodeStates(wi.getChain());

        Test::check(
            "Document verification passed",
            wi.getChain()->getStateName() == "Passed");

        Test::check(
            "Eligibility check passed",
            wi.getChain()->getNext()->getStateName() == "Passed");

        Test::check(
            "Workflow reached Approved status",
            wi.isFinished());

        Test::check(
            "Observer received approval notification",
            observer->received(Outcome::Approved));

        Test::detail(
            "The second node executed automatically after the first passed.");
    }

    // --------------------------------------------------------
    // 3. REJECTION STOPS THE CHAIN
    // --------------------------------------------------------

    Test::heading("3. REJECTION AND CHAIN TERMINATION");

    {
        std::shared_ptr<Document> badDocument = makeDocument(
            {
                {"name", "Jane Doe"},
                {"address", ""},
                {"email", "jane@example.com"}
            },
            {
                {"age", 25.0},
                {"salary", 40000.0},
                {"score", 90.0}
            }
        );

        WorkflowInstance wi("WF-002", "Application", badDocument);

        wi.setChain(makeStandardChain());

        std::shared_ptr<RecordingObserver> observer(
            new RecordingObserver());

        wi.attach(observer);

        wi.run();

        std::cout << "\n  EXECUTION RESULTS\n";
        Test::nodeStates(wi.getChain());

        Test::check(
            "Incomplete document was rejected",
            wi.getChain()->getStateName() == "Rejected");

        Test::check(
            "Next node was not executed",
            wi.getChain()->getNext()->getStateName() == "Waiting");

        Test::check(
            "Workflow recorded the rejection point",
            wi.getRejectedAt() == "DocumentVerification");

        Test::check(
            "Observer received rejection notification",
            observer->received(Outcome::Rejected));
    }

    // --------------------------------------------------------
    // 4. DOCUMENT VALIDATION
    // --------------------------------------------------------

    Test::heading("4. DOCUMENT VALIDATION");

    {
        std::shared_ptr<Document> badDocument = makeDocument(
            {
                {"name", "Invalid Applicant"},
                {"address", "Somewhere"},
                {"email", "invalid@example.com"}
            },
            {
                {"age", 25.0},
                {"salary", std::nan("")},
                {"score", 80.0}
            }
        );

        WorkflowInstance wi("WF-003", "Application", badDocument);

        std::unique_ptr<WorkflowNode> head(
            new DocCompletionCheckNode("DocumentVerification"));

        wi.setChain(std::move(head));

        wi.run();

        Test::check(
            "NaN comparable value was rejected",
            wi.getChain()->getStateName() == "Rejected");
    }

    // --------------------------------------------------------
    // 5. RANGE VALIDATION
    // --------------------------------------------------------

    Test::heading("5. ELIGIBILITY / RANGE VALIDATION");

    {
        std::shared_ptr<Document> badAge = makeDocument(
            {
                {"name", "Underage Applicant"},
                {"address", "Somewhere"},
                {"email", "age@example.com"}
            },
            {
                {"age", 15.0},
                {"salary", 50000.0},
                {"score", 85.0}
            }
        );

        WorkflowInstance wi("WF-004", "Application", badAge);

        std::unique_ptr<WorkflowNode> head(
            new RangeCheckNode("EligibilityCheck", standardRules()));

        wi.setChain(std::move(head));
        wi.run();

        Test::check(
            "Age below minimum was rejected",
            wi.getChain()->getStateName() == "Rejected");
    }

    {
        std::shared_ptr<Document> missingSalary = makeDocument(
            {
                {"name", "Applicant Without Salary"},
                {"address", "Somewhere"},
                {"email", "salary@example.com"}
            },
            {
                {"age", 30.0},
                {"score", 85.0}
            }
        );

        WorkflowInstance wi("WF-005", "Application", missingSalary);

        std::unique_ptr<WorkflowNode> head(
            new RangeCheckNode("EligibilityCheck", standardRules()));

        wi.setChain(std::move(head));
        wi.run();

        Test::check(
            "Missing comparable key was rejected",
            wi.getChain()->getStateName() == "Rejected");
    }

    // --------------------------------------------------------
    // 6. PENDING APPROVAL AND RESUME
    // --------------------------------------------------------

    Test::heading("6. PENDING APPROVAL AND RESUMPTION");

    {
        WorkflowInstance wi("WF-006", "Application", document);

        std::unique_ptr<WorkflowNode> head(
            new ApprovalTestNode("HumanApproval"));

        std::unique_ptr<RangeCheckNode> next(
            new RangeCheckNode("EligibilityCheck", standardRules()));

        head->setNext(std::move(next));

        wi.setChain(std::move(head));

        wi.run();

        std::cout << "\n  BEFORE APPROVAL\n";
        Test::nodeStates(wi.getChain());

        Test::check(
            "Human approval node entered Pending state",
            wi.getChain()->getStateName() == "Pending");

        Test::check(
            "Next node remained Waiting",
            wi.getChain()->getNext()->getStateName() == "Waiting");

        WorkflowNode* approvalNode = wi.getCurrent();

        approvalNode->approve();

        wi.resume();

        std::cout << "\n  AFTER APPROVAL AND RESUMPTION\n";
        Test::nodeStates(wi.getChain());

        Test::check(
            "Human approval node transitioned to Passed",
            wi.getChain()->getStateName() == "Passed");

        Test::check(
            "Next node executed after resumption",
            wi.getChain()->getNext()->getStateName() == "Passed");

        Test::check(
            "Workflow completed after resumption",
            wi.isFinished());
    }

    // --------------------------------------------------------
    // 7. PENDING APPROVAL REJECTION
    // --------------------------------------------------------

    Test::heading("7. REJECTION OF A PENDING APPROVAL");

    {
        WorkflowInstance wi("WF-007", "Application", document);

        std::unique_ptr<WorkflowNode> head(
            new ApprovalTestNode("HumanApproval"));

        wi.setChain(std::move(head));

        std::shared_ptr<RecordingObserver> observer(
            new RecordingObserver());

        wi.attach(observer);

        wi.run();

        WorkflowNode* current = wi.getCurrent();

        current->reject();
        wi.reject(current->getName());

        Test::check(
            "Pending node transitioned to Rejected",
            current->getStateName() == "Rejected");

        Test::check(
            "Workflow status became Rejected",
            wi.isFinished());

        Test::check(
            "Observer received rejection notification",
            observer->received(Outcome::Rejected));
    }

    // --------------------------------------------------------
    // 8. CANCELLATION
    // --------------------------------------------------------

    Test::heading("8. WORKFLOW CANCELLATION");

    {
        WorkflowInstance wi("WF-008", "Application", document);

        std::unique_ptr<WorkflowNode> head(
            new ApprovalTestNode("HumanApproval"));

        wi.setChain(std::move(head));

        std::shared_ptr<RecordingObserver> observer(
            new RecordingObserver());

        wi.attach(observer);

        wi.run();

        CancelWorkflowCommand command(&wi);

        bool commandResult = command.execute();

        Test::check(
            "Cancellation command executed",
            commandResult);

        Test::check(
            "Workflow status became Cancelled",
            wi.isCancelled());

        Test::check(
            "Observer received cancellation notification",
            observer->received(Outcome::Cancelled));

        Test::check(
            "Current node transitioned to Cancelled",
            wi.getChain()->getStateName() == "Cancelled");

        ResetWorkflowCommand resetCommand(&wi);

        Test::check(
            "Cancelled workflow cannot be reset",
            resetCommand.execute() == false);
    }

    // --------------------------------------------------------
    // 9. RESET AND RE-EXECUTION
    // --------------------------------------------------------

    Test::heading("9. WORKFLOW RESET AND RE-EXECUTION");

    {
        WorkflowInstance wi("WF-009", "Application", document);

        wi.setChain(makeStandardChain());

        wi.run();

        Test::check(
            "Workflow completed before reset",
            wi.isFinished());

        ResetWorkflowCommand command(&wi);

        bool resetResult = command.execute();

        Test::check(
            "Reset command executed",
            resetResult);

        Test::check(
            "First node returned to Waiting",
            wi.getChain()->getStateName() == "Waiting");

        Test::check(
            "Second node returned to Waiting",
            wi.getChain()->getNext()->getStateName() == "Waiting");

        wi.run();

        Test::check(
            "Workflow successfully executed again",
            wi.isFinished());

        Test::check(
            "Both nodes passed after re-execution",
            wi.getChain()->getStateName() == "Passed" &&
            wi.getChain()->getNext()->getStateName() == "Passed");
    }

    // --------------------------------------------------------
    // 10. FAILURE HANDLING
    // --------------------------------------------------------

    Test::heading("10. PROCESSING FAILURE HANDLING");

    {
        WorkflowInstance wi("WF-010", "Application", document);

        std::unique_ptr<WorkflowNode> head(
            new FailingTestNode("SimulatedFailure"));

        wi.setChain(std::move(head));

        wi.run();

        Test::check(
            "Exception was caught by WorkflowNode::handle",
            wi.getChain()->getStateName() == "Failed");

        Test::check(
            "Failure was recorded in workflow history",
            !wi.getHistory().empty() &&
            wi.getHistory().back().message ==
                "Simulated processing failure");
    }

    // --------------------------------------------------------
    // 11. OBSERVER NOTIFICATIONS
    // --------------------------------------------------------

    Test::heading("11. OBSERVER NOTIFICATION SYSTEM");

    {
        std::shared_ptr<RecordingObserver> observer(
            new RecordingObserver());

        WorkflowInstance wi("WF-011", "Application", document);

        wi.attach(observer);

        wi.setChain(makeStandardChain());
        wi.run();

        Test::check(
            "Observer received completion notification",
            observer->received(Outcome::Approved));

        Test::check(
            "Observer received exactly one notification",
            observer->notificationCount() == 1);

        wi.detach(observer);

        // A second workflow confirms the detached observer is not notified.
        WorkflowInstance second("WF-012", "Application", document);

        second.setChain(makeStandardChain());
        second.run();

        Test::check(
            "Detached observer did not receive another notification",
            observer->notificationCount() == 1);
    }

    // --------------------------------------------------------
    // 12. WORKFLOW HISTORY
    // --------------------------------------------------------

    Test::heading("12. WORKFLOW EXECUTION HISTORY");

    {
        WorkflowInstance wi("WF-013", "Application", document);

        wi.setChain(makeStandardChain());
        wi.run();

        const std::vector<HistoryEntry>& history = wi.getHistory();

        std::cout << "\n  RECORDED HISTORY\n";

        for (std::size_t i = 0; i < history.size(); ++i) {
            std::cout << "  " << std::setw(2) << i + 1
                      << ". " << std::left << std::setw(25)
                      << history[i].nodeName
                      << std::setw(12) << history[i].stateName
                      << history[i].message << "\n";
        }

        Test::check(
            "History contains entries for both executed nodes",
            history.size() == 2);

        Test::check(
            "First history entry belongs to document verification",
            history.size() >= 1 &&
            history[0].nodeName == "DocumentVerification");

        Test::check(
            "Second history entry belongs to eligibility check",
            history.size() >= 2 &&
            history[1].nodeName == "EligibilityCheck");
    }

    // --------------------------------------------------------
    // 13. WORKFLOW DEFINITION
    // --------------------------------------------------------

    Test::heading("13. WORKFLOW DEFINITION");

    {
        WorkflowDefinition definition("Application");

        std::unique_ptr<WorkflowNode> head(
            new DocCompletionCheckNode("DocumentVerification"));

        definition.setHead(std::move(head));

        Test::check(
            "Workflow definition stores its head node",
            definition.getHead() != nullptr);

        Test::check(
            "Workflow definition returns the correct head",
            definition.getHead()->getName() ==
                "DocumentVerification");
    }

    // --------------------------------------------------------
    // 14. NODE CLONING
    // --------------------------------------------------------

    Test::heading("14. NODE CLONING");

    {
        DocCompletionCheckNode original("OriginalDocumentNode");

        std::unique_ptr<WorkflowNode> copy = original.clone();

        Test::check(
            "Document node clone was created",
            copy.get() != nullptr);

        Test::check(
            "Document node clone preserved its name",
            copy->getName() == "OriginalDocumentNode");

        RangeCheckNode originalRange(
            "OriginalRangeNode", standardRules());

        std::unique_ptr<WorkflowNode> rangeCopy =
            originalRange.clone();

        Test::check(
            "Range node clone was created",
            rangeCopy.get() != nullptr);

        Test::check(
            "Range node clone preserved its name",
            rangeCopy->getName() == "OriginalRangeNode");
    }

    // --------------------------------------------------------
    // 15. CHAIN CLONING
    // --------------------------------------------------------

    Test::heading("15. COMPLETE CHAIN CLONING");
    {
        std::unique_ptr<WorkflowNode> original = makeStandardChain();

        std::unique_ptr<WorkflowNode> copy =
            original->cloneChain();

        Test::check(
            "Cloned chain has a head node",
            copy.get() != nullptr);

        Test::check(
            "Cloned chain has the correct first node",
            copy->getName() == "DocumentVerification");

        Test::check(
            "Cloned chain contains the second node",
            copy->getNext() != nullptr);

        Test::check(
            "Cloned chain preserved the second node's name",
            copy->getNext()->getName() == "EligibilityCheck");

        Test::check(
            "Cloned nodes begin in Waiting state",
            copy->getStateName() == "Waiting" &&
            copy->getNext()->getStateName() == "Waiting");
    }
    runUniversityApplicationDemo();
    Test::summary();

    return Test::failed == 0 ? 0 : 1;
}
