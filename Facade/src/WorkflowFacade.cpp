#include "WorkflowFacade.h"

#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "Document.h"
#include "Director.h"
#include "WorkflowInstance.h"
#include "WorkflowNode.h"

#include "WorkflowObserver.h"

#include "ui_logger.hpp"

namespace {

const char* outcomeName(Outcome outcome) {
    switch (outcome) {
        case Outcome::Approved: return "Approved";
        case Outcome::Rejected: return "Rejected";
        case Outcome::Cancelled: return "Cancelled";
        case Outcome::Failed: return "Failed";
    }
    return "Unknown";
}

class RecordingObserver : public WorkflowObserver {
private:
    std::vector<Outcome> outcomes;

public:
    void onWorkflowFinished(WorkflowInstance& workflow, Outcome outcome) override {
        outcomes.push_back(outcome);
        UI_LOG("[OBSERVER] RecordingObserver received " + std::string(outcomeName(outcome)) +
               " for workflow '" + workflow.getId() + "'");
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

void logLine(const std::string& label, const std::string& value) {
    std::ostringstream oss;
    oss << std::left << std::setw(23) << label << value;
    UI_LOG("[WF] " + oss.str());
}




}

WorkflowFacade::WorkflowFacade(
    const std::string& workflowType,
    const std::string& idPrefix,
    std::function<void(Document&, const std::string&)> prepareApplication)
    : document(new Document()), builder(document.get(), &tui),
      workflowType(workflowType), idPrefix(idPrefix),
      prepareApplication(std::move(prepareApplication)), applicationCounter(0) {}

WorkflowFacade::~WorkflowFacade() = default;

void WorkflowFacade::prepareDocument(
    const std::vector<std::string>& commonFields,
    const std::vector<std::shared_ptr<Comparable> >& comparableFields,
    const std::map<std::string, std::string>& initialValues)
{
    director.reset(new Director(&builder, commonFields, comparableFields));
    director->prepare();
    tui.setValues(initialValues);
}

void WorkflowFacade::addNode(std::unique_ptr<WorkflowNode> node) {
    if (!node) {
        throw std::invalid_argument("Cannot add a null workflow node");
    }
    if (!workflowHead) {
        workflowHead = std::move(node);
        return;
    }

    WorkflowNode* last = workflowHead.get();
    while (last->getNext()) {
        last = last->getNext();
    }
    last->setNext(std::move(node));
}

void WorkflowFacade::onApproved(std::function<void(WorkflowInstance&)> action) {
    approvedAction = std::move(action);
}

int WorkflowFacade::run() {
    if (!director || !workflowHead) {
        throw std::logic_error("Prepare the document and add at least one node before running");
    }
    UI_LOG("[TUI] Starting scenario: " + workflowType);
    tui.run([this] { submitApplication(); }, workflowType);
    return 0;
}

void WorkflowFacade::submitApplication() {
    director->construct();
    const std::string applicationNumber = std::to_string(++applicationCounter);

    if (prepareApplication) {
        prepareApplication(*document, applicationNumber);
    }

    WorkflowInstance workflow(
        idPrefix + applicationNumber, workflowType, document
    );
    workflow.setChain(workflowHead->cloneChain());

    std::shared_ptr<RecordingObserver> observer(new RecordingObserver());
    workflow.attach(observer);

    logLine("Applicant fields:", "copied from Document Builder");
    logLine("Workflow:", workflow.getId());
    workflow.run();

    if (workflow.isFinished() || workflow.isCancelled()) {
        const Outcome expected = workflow.isFailed() ? Outcome::Failed
            : workflow.isCancelled() ? Outcome::Cancelled
            : !workflow.getRejectedAt().empty() ? Outcome::Rejected
            : Outcome::Approved;
        const bool notifiedOnce = observer->notificationCount() == 1 && observer->received(expected);
        UI_LOG(std::string(notifiedOnce ? "[PASS] " : "[FAIL] ") +
               "Observer notification check for " + outcomeName(expected) +
               ": received " + std::to_string(observer->notificationCount()) +
               " notification(s), expected exactly one with the matching outcome");
    }

    UI_LOG("[WF] NODE RESULTS");
    for (WorkflowNode* node = workflow.getChain(); node; node = node->getNext()) {
        UI_LOG("[WF]   " + node->getName() + " -> " + node->getStateName());
    }

    if (workflow.isFailed()) {
        UI_LOG("[FAIL] Technical failure at " + workflow.getCurrent()->getName() +
               "; the application was not rejected on workflow conditions");
    } else if (!workflow.getRejectedAt().empty()) {
        UI_LOG("[FAIL] Workflow rejected at " + workflow.getRejectedAt());
    } else if (workflow.isCancelled()) {
        UI_LOG("[WF] Workflow cancelled");
    } else if (workflow.isFinished() &&
               observer->notificationCount() == 1 &&
               observer->received(Outcome::Approved)) {
        UI_LOG("[PASS] Application approved and observer notified exactly once");
        if (approvedAction) {
            approvedAction(workflow);
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
}


