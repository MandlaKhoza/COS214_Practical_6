
#ifndef NODESTATE_H
#define NODESTATE_H

#include <string>

class WorkflowNode;

/**
 * @brief State (abstract): one node's progress.
 *
 * Every event defaults to invalid(), which throws std::logic_error.
 * Each concrete state overrides only the events it allows.
 */
class NodeState {
public:
    virtual ~NodeState() {}

    virtual std::string getName() const = 0;
    /// True when the node's work is finished (Passed, Cancelled).
    virtual bool isFinal() const { return false; }

    virtual void start(WorkflowNode& node);          ///< Waiting → InProgress
    virtual void pass(WorkflowNode& node);           ///< InProgress → Passed
    virtual void needsApproval(WorkflowNode& node);  ///< InProgress → Pending
    virtual void approve(WorkflowNode& node);        ///< Pending → Passed
    virtual void reject(WorkflowNode& node);         ///< InProgress / Pending → Rejected
    virtual void fail(WorkflowNode& node);           ///< InProgress → Failed
    virtual void reset(WorkflowNode& node);          ///< Rejected / Failed / Passed → Waiting
    virtual void cancel(WorkflowNode& node);         ///< any non-final → Cancelled

protected:
    /// Throws std::logic_error("Cannot <action> while <name>").
    void invalid(const std::string& action) const;
};

#endif
