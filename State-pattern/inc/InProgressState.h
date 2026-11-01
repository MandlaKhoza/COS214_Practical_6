#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "NodeState.h"

/// @brief Running its check. → Passed / Pending / Rejected / Failed / Cancelled.
class InProgressState : public NodeState {
	public:
		std::string getName() const;

		void pass(WorkflowNode& node);

		void needsApproval(WorkflowNode& node);

		void reject(WorkflowNode& node);

		void fail(WorkflowNode& node);
};

#endif
