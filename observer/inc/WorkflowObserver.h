#ifndef WORKFLOWOBSERVER_H
#define WORKFLOWOBSERVER_H

#include "WorkflowEnums.h"

class WorkflowInstance;

/**
 * @brief Abstract Observer in the Observer pattern.
 *
 * Classes that need to react when a workflow finishes inherit from this.
 */
class WorkflowObserver {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~WorkflowObserver() = default;

    /**
     * @brief Called when the observed workflow instance finishes.
     * @param wi The workflow instance that finished.
     * @param out How the workflow finished.
     */
    virtual void onWorkflowFinished(WorkflowInstance& wi, Outcome out) = 0;
};

#endif
