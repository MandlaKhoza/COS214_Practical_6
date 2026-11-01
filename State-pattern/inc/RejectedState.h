#ifndef REJECTEDSTATE_H
#define REJECTEDSTATE_H

#include "NodeState.h"

/// @brief Check failed; chain stopped. reset → Waiting (Command gives a fresh start).
class RejectedState : public NodeState {

	public:
		std::string getName() const;
		void reset(WorkflowNode& node);
};

#endif
