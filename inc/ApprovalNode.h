#ifndef APPROVALNODE_H
#define APPROVALNODE_H


#include <memory>
#include <string>

#include "WorkflowNode.h"



class ApprovalChain;


/**
 * @brief Link between Chain of Responsibility 1 and Chain of Responsibility 2.
 *
 * A workflow node that hands its instance to an ApprovalChain.
 * Approved becomes Passed, and Rejected becomes Rejected. The approval
 * records are copied into the instance's history.
 * @see ApprovalChain
 */
class ApprovalNode : public WorkflowNode {

	private:
		/// @brief The approval chain this node uses. Shared, because approvers are stateless.
		std::shared_ptr<ApprovalChain> approvals;

	public:
		/**
 		* @brief Creates an approval node.
 		* @param n  The node's name.
 		* @param a  The approval chain to run.
 		*/
		ApprovalNode(const std::string& name, const std::shared_ptr<ApprovalChain> approvals);

		/**
 		* @brief Copies this node for a new workflow instance (Prototype).
 		* @return A new node in the Waiting state that shares the same approval chain.
 		*/
		std::unique_ptr<WorkflowNode> clone() const override;

	protected:
		/**
 		* @brief Runs the approval chain on the instance.
 		* @param wi  The workflow instance being processed.
 		* @return Passed if the final decision is Approved, otherwise Rejected,
 		*         with a message naming who decided.
 		*/
		NodeResult process(WorkflowInstance& wi) override;
};

#endif
