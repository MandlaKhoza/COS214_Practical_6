#ifndef APPROVER_H
#define APPROVER_H

#include <string>
#include <memory>
#include "ApprovalRequest.h"
#include "ApprovalDecision.h"

/**
    @brief Abstract handler of Chain of Responsibility 2 
    * approve() is the non-virtual chain logic
    * decide() is the step each subclass overrides
    * Each approver owns the next through a next unique_ptr, so the chain cleans itself up
*/
class Approver 
{
    //members
    private:
        /// @brief this is the name of the approver 
        std::string name;
        /// @brief this is the role of the approver
        std::string role;
        /// @brief this is each approver's next, if it has one
        std::unique_ptr<Approver> next;
        

    //functions
    public:
        /**
         * @brief Constructor, creates an approver object
         * @param n the approver's name
         * @param r the approver's role
         */
        Approver(const std::string& n, const std::string& r);

        /**
         * @brief default destructor
         */
        virtual ~Approver() = default;

        /**
         * @brief takes ownership of the next approver and returns a borrowed pointer for chaining
         * @param n the next approver
         * @return returns the approver that was just added
         */
        Approver* setNext(std::unique_ptr<Approver> n);

        /**
         * @brief records the decision, then stops or forwards
         * @param req the request that is handled by the approver
         * @return the final decision of the request is returned: either Approved or Rejected
         */
        ApprovalDecision approve(ApprovalRequest& req);

    protected:
        /**
         * @brief the subclasses must override this
         * @param req the request that an approver must make a decision on
         * @return returns the decision an approver made: Approved, Rejected, Escalate
         */
        virtual ApprovalDecision decide(const ApprovalRequest& req) const = 0;

    public:
        /**
         * @brief returns the name of the approver
         */
        const std::string& getName() const;

        /**
         * @brief returns the role of the approver
         */
        const std::string& getRole() const;

        /**
         * @brief returns the next approver in the chain
         * @return returns a nullptr at the end of the chain which must not be deleted by the caller
         */
        Approver* getNext() const;
};

#endif