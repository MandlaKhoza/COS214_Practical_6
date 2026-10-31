/**
 * @file AppealCommand.h
 * @brief Command: resubmits a rejected application with corrected details.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef APPEALCOMMAND_H
#define APPEALCOMMAND_H

#include <map>
#include <string>
#include "Command.h"

class WorkflowInstance;

/**
 * @class AppealCommand
 * @brief ConcreteCommand: resubmits a rejected application with corrected details.
 *
 * The applicant corrects the fields that caused the rejection. The command applies
 * the corrections to the Document (reusing AnswerDocumentBuilder::setMapValues),
 * resets the workflow (reusing ResetWorkflowCommand) and runs every check again,
 * so the outcome is decided on the new data.
 *
 * ### Example Usage:
 * @code
 * std::map<std::string, std::string> corrections = {{FieldNames::MONTHLY_INCOME, "32000"}};
 * controller.execute(std::unique_ptr<Command>(new AppealCommand(instance, corrections)));
 * @endcode
 *
 * @note One appeal per application. The original rejection stays in the history.
 * @see ResetWorkflowCommand, AnswerDocumentBuilder, WorkflowController, TividyFacade::appeal
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class AppealCommand: public Command{
    public:
        /**
         * @brief Creates the appeal.
         * @param[in] instance       The rejected workflow instance (not owned).
         * @param[in] updatedAnswers Field name → corrected value; copied, so the caller's map may be temporary.
         */
        AppealCommand(WorkflowInstance* instance, const std::map<std::string, std::string>& updatedAnswers);

        /**
         * @brief Applies the corrections, resets the workflow and runs it again.
         *
         * All refusal checks happen before anything changes, so a refused appeal
         * leaves the application exactly as it was.
         *
         * @pre The instance's status is Rejected and it has not been appealed before.
         * @post On success: the instance is marked appealed, a history entry is added,
         *       and the workflow has run again on the corrected Document.
         * @retval true  The appeal was lodged and the workflow ran again.
         * @retval false Refused: no instance, not Rejected, already appealed, or no corrections given.
         * @throws std::logic_error Passed up from the workflow if a state change is refused;
         *         the WorkflowController catches it and logs the command as failed.
         */
        bool execute();

        /**
         * @brief Describes the command for the audit log.
         * @return e.g. "Appeal workflow LoanRequest-0007".
         */
        std::string describe() const;

    private:
        WorkflowInstance* instance;                          ///< the receiver (not owned)
        std::map<std::string, std::string> updatedAnswers;  ///< the applicant's corrections (a copy)
};

#endif
