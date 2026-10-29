#include "NotificationObserver.h"
#include "MessageSender.h"
#include "WorkflowInstance.h"
#include "Document.h"

NotificationObserver::NotificationObserver(std::shared_ptr<MessageSender> sender, std::string recipientField):sender(sender), recipientField(recipientField) {}

void NotificationObserver::onWorkflowFinished(WorkflowInstance& wi, Outcome out){
    std::string recipient;
    if(!wi.getDocument()->getField(recipientField, recipient)) {
        recipient = "unknown";
    }
    sender->send(recipient, "Update on " + wi.getId(), buildMessage(wi, out));
}

 std::string NotificationObserver::buildMessage(const WorkflowInstance& wi, Outcome out) const{
    if(out == Outcome::Approved){
        return "Your application " + wi.getId() + " has been approved.";
    }
    if(out == Outcome::Rejected){
        return "Your application " + wi.getId() + " was rejected at " + wi.getRejectedAt() + ".";
    }
        return "Your application " + wi.getId() + " was unfortunately cancelled.";
}
