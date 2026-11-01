#ifndef WORKFLOWSUBJECT_H
#define WORKFLOWSUBJECT_H

#include <memory>
#include <vector>
#include "WorkflowObserver.h"

/**
 * @brief Subject in the Observer pattern.
 *
 * Stores a list of observers and notifies them when a workflow finishes.
 * WorkflowInstance inherits from this class.
 */
class WorkflowSubject {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~WorkflowSubject() = default;

    /**
     * @brief Adds an observer to the list.
     * @param o The observer to add. Null observers are ignored.
     */
    void attach(std::shared_ptr<WorkflowObserver> o);

    /**
     * @brief Removes an observer from the list.
     * @param o The observer to remove.
     */
    void detach(const std::shared_ptr<WorkflowObserver> o);

protected:
    /**
     * @brief Notifies every observer, in the order they were attached.
     * @param wi The workflow instance that finished.
     * @param out How the workflow finished.
     */
    void notify(WorkflowInstance* wi, Outcome out);

private:
    std::vector<std::shared_ptr<WorkflowObserver>> observers; ///< The attached observers.
};

#endif
