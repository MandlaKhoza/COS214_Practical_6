#ifndef MESSAGESENDER_H
#define MESSAGESENDER_H

#include <string>

/**
 * @brief Sends messages to applicants.
 *
 * Stands in for a real email service by printing to the console.
 */
class MessageSender {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~MessageSender() = default;

    /**
     * @brief Sends a message.
     * @param recipient Who the message is sent to.
     * @param subject The subject line.
     * @param body The message text.
     */
    virtual void send(const std::string& recipient, const std::string& subject, const std::string& body);
};

#endif
