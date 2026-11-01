#ifndef WORKFLOW_INSTANCE_H
#define WORKFLOW_INSTANCE_H

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../../DocumentBuilder/inc/Document.h"
#include "WorkflowNode.h"
#include "NodeResult.h"

//History "nodes"
struct HistoryEntry{
    std::string timestamp;
    std::string nodeName;
    std::string stateName;
    std::string message;
};

#include "../../observer/inc/WorkflowEnums.h"    // Outcome (Observer pattern)
#include "../../observer/inc/WorkflowSubject.h"

/// @brief Where a workflow instance is in its life (the only definition in the project).
enum class WorkflowStatus { Created, Running, Approved, Rejected, Cancelled };

// Subject of the Observer pattern: observers are told when the workflow finishes
class WorkflowInstance : public WorkflowSubject {
private:
    std::string id;
    std::shared_ptr<Document> document;
    std::vector<HistoryEntry> history;
    WorkflowStatus status;
    std::unique_ptr<WorkflowNode> chain;
    WorkflowNode* current;
    std::string rejectedAt;
    std::string type;
public:
   WorkflowInstance(const std::string& id,
                 const std::string& type,
                 std::shared_ptr<Document> document);
    ~WorkflowInstance();

    const std::string& getId() const;
    std::shared_ptr<Document> getDocument() const;
    void setChain(std::unique_ptr<WorkflowNode> chain);
    WorkflowNode* getChain() const; // EXTRA : Remove if unused
    void run();
    void resume();
    void reset();
    void setCurrent(WorkflowNode* node);
    WorkflowNode* getCurrent() const;
    void addHistory(const std::string& nodeName,
                    const std::string& message);
    const std::vector<HistoryEntry>& getHistory() const;
    void complete();
    void cancel();
    bool isFinished() const;
    bool isCancelled() const;
    void reject(const std::string& nodeName); // EXTRA : Remove if unused
    void markRunning(); // Remove if unused
    const std::string& getRejectedAt() const;
    const std::string& getType() const;
};

#endif
