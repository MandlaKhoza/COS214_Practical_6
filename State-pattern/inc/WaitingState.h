#ifndef WAITINGSTATE_H
#define WAITINGSTATE_H

#include "NodeState.h"

/// @brief Not reached yet. start → InProgress; reset → stays Waiting; cancel → Cancelled.
class WaitingState : public NodeState {
public:
    std::string getName() const override;
    void start(WorkflowNode& node) override;
    void reset(WorkflowNode& node) override;
    void cancel(WorkflowNode& node) override;
};

#endif
