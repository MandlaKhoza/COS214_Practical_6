#ifndef WORKFLOWFACADE_H
#define WORKFLOWFACADE_H


#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "DocumentBuilder.h"

#include "tui_app.hpp"

class Director;
class WorkflowNode;

class WorkflowInstance;

/**
 * @brief Runs client-configured workflows and optional post-approval actions.
 */
class WorkflowFacade {
private:
    TUI tui;
    std::shared_ptr<Document> document;
    DocumentBuilder builder;
    std::unique_ptr<Director> director;
    std::unique_ptr<WorkflowNode> workflowHead;
    std::string workflowType;
    std::string idPrefix;
    std::function<void(Document&, const std::string&)> prepareApplication;
    std::function<void(WorkflowInstance&)> approvedAction;
    unsigned int applicationCounter;

    void submitApplication();


public:
    WorkflowFacade(
        const std::string& workflowType,
        const std::string& idPrefix,
        std::function<void(Document&, const std::string&)> prepareApplication = {});
    ~WorkflowFacade();

    void prepareDocument(
        const std::vector<std::string>& commonFields,
        const std::vector<std::shared_ptr<Comparable> >& comparableFields,
        const std::map<std::string, std::string>& initialValues = {});

    /// @brief Appends a node in client-specified order; ownership passes to the facade.
    void addNode(std::unique_ptr<WorkflowNode> node);

    /// @brief Client-selected action; no account is created unless the client requests it.
    void onApproved(std::function<void(WorkflowInstance&)> action);

    int run();
};

#endif
