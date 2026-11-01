#include "../inc/MessageSender.h"
#include "ui_logger.hpp"

void MessageSender::send(const std::string& recipient, const std::string& subject, const std::string& body){
    UI_LOG("[NOTIFY] To: " + recipient);
    UI_LOG("[NOTIFY] Subject: " + subject);
    UI_LOG("[NOTIFY] " + body);
}
