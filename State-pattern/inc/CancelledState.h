#ifndef CANCELLEDSTATE_H
#define CANCELLEDSTATE_H

#include "NodeState.h"

/// @brief Applicant withdrew. Final; every event is refused.
class CancelledState : public NodeState {
public:
    std::string getName() const;
    bool isFinal() const { return true; }
};

#endif
