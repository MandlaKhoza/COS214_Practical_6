#ifndef PENDINGSTATE_H
#define PENDINGSTATE_H

#include "NodeState.h"

/// @brief Waiting for a human decision. → Passed / Rejected / Cancelled.
class PendingState : public NodeState {
public:
    std::string getName() const;
    void approve(WorkflowNode& node);
    void reject(WorkflowNode& node);
    void cancel(WorkflowNode& node);
};

#endif
