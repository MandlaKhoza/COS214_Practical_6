#ifndef FAILEDSTATE_H
#define FAILEDSTATE_H

#include "NodeState.h"

/// @brief Technical error. retry / reset → Waiting; cancel → Cancelled.
class FailedState : public NodeState {
public:
    std::string getName() const;
    void retry(WorkflowNode& node);
    void reset(WorkflowNode& node);
    void cancel(WorkflowNode& node);
};

#endif
