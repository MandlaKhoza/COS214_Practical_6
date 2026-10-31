#ifndef WORKFLOW_INSTANCE_H
#define WORKFLOW_INSTANCE_H

#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../DocumentBuilder/Document.h"
#include "WorkflowNode.h"
#include "NodeResult.h"

//History "nodes"
struct HistoryEntry{
    std::string timestamp;
    std::string nodeName;
    std::string stateName;
    std::string message;
};

//Workflow status enum
enum WorkflowStatus {
    Created,
    Running,
    Approved,
    Rejected,
    Cancelled
};

class WorkflowInstance {
private:
    std::string id;
    std::shared_ptr<Document> document;
    std::vector<HistoryEntry> history;
    WorkflowStatus status;
    std::unique_ptr<WorkflowNode> chain;
    WorkflowNode* current;
    std::string rejectedAt;

public:
    WorkflowInstance(const std::string& id,
                     std::shared_ptr<Document> document);
    ~WorkflowInstance();

    std::shared_ptr<Document> getDocument() const;
    void setChain(std::unique_ptr<WorkflowNode> chain);
    WorkflowNode* getChain() const;
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
    void rejectNodeName(const std::string& nodeName);
    void markRunning();
    const std::string& getRejectedAt() const;
};

#endif
