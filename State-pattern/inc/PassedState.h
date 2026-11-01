#ifndef PASSEDSTATE_H
#define PASSEDSTATE_H

#include "NodeState.h"

/// @brief Check succeeded. Final; reset → Waiting (whole-chain reset only).
class PassedState : public NodeState {
public:
    std::string getName() const;
    bool isFinal() const { return true; }
    void reset(WorkflowNode& node);
};

#endif
