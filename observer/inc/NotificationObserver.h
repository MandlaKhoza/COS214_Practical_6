#ifndef NOTIFICATIONOBSERVER_H
#define NOTIFICATIONOBSERVER_H

#include <memory>
#include <string>
#include "WorkflowObserver.h"

class MessageSender;

/**
 * @brief Concrete Observer that notifies the applicant of the outcome.
 */
class NotificationObserver : public WorkflowObserver {
public:
    /**
     * @brief Constructor.
     * @param sender Used to send the message.
     * @param recipientField The document field holding the recipient, e.g. "email".
     */
    NotificationObserver(std::shared_ptr<MessageSender> sender, std::string recipientField);

    /**
     * @brief Sends the applicant a message about how the workflow finished.
     * @param wi The workflow instance that finished.
     * @param out How the workflow finished.
     */
    void onWorkflowFinished(WorkflowInstance& wi, Outcome out) override;

private:
    /**
     * @brief Builds the message text for an outcome.
     * @param wi The workflow instance that finished.
     * @param out How the workflow finished.
     * @return The message text.
     */
    std::string buildMessage(const WorkflowInstance& wi, Outcome out) const;

    std::shared_ptr<MessageSender> sender; ///< Sends the message.
    std::string recipientField;            ///< Document field holding the recipient.
};

#endif
