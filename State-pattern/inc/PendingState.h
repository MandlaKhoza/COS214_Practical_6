#ifndef PENDINGSTATE_H
#define PENDINGSTATE_H

#include "NodeState.h"

/// @brief Waiting for a human decision. → Passed / Rejected / Cancelled / Failed.
class PendingState : public NodeState {
public:
    std::string getName() const;
    void approve(WorkflowNode& node);
    void reject(WorkflowNode& node);
    void fail(WorkflowNode& node) override;
    void cancel(WorkflowNode& node);
    void reset(WorkflowNode& node);
};

#endif
