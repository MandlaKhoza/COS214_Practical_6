#include "WorkflowInstance.h"

WorkflowInstance::WorkflowInstance(const std::string& id, std::shared_ptr<Document> document){
    this->id = id;
    this->document = document;
}

WorkflowInstance::~WorkflowInstance() = default;

std::shared_ptr<Document> WorkflowInstance::getDocument() const {
    return this->document;
}

void WorkflowInstance::setChain(std::unique_ptr<WorkflowNode> chain){
    this->chain = std::move(chain);
}

WorkflowNode* WorkflowInstance::getChain() const{
    return chain.get();
}

void WorkflowInstance::run(){
    HandleResult result = this->chain->handle(this);
    //Principle, this is how it will "look like"
}

void WorkflowInstance::resume(){
//Most likely does the same as run
}

void WorkflowInstance::reset(){
    if(status == WorkflowStatus::Cancelled) throw std::logic_error("Cannot reset a cancelled workflow");

    status = WorkflowStatus::Created;
    current = chain.get();
    rejectedAt.clear();
    addHistory("Workflow", "Reset: all steps back to Waiting, ready to run again");
}

void WorkflowInstance::setCurrent(WorkflowNode* node){
    this->current = node;
    //Apparently called by the ConcreteHandlers
}

WorkflowNode* WorkflowInstance::getCurrent() const{
    return this->current;
}

void WorkflowInstance::addHistory(const std::string& nodeName, const std::string& message){
 //Not finished, timestampe and state has to be sorted
 HistoryEntry object;
 object.timestamp = "10:00"; //It's always 10:00, what do you mean?
 object.nodeName = nodeName;
 object.stateName = this->status; // Does enum convert to string?
 object.message = message;

 this->history.push_back(object);
}

const std::vector<HistoryEntry>& WorkflowInstance::getHistory() const{
    return this->history;
}

void WorkflowInstance::complete(){
 //TODO
}

void WorkflowInstance::cancel(){
 //TODO
}

bool WorkflowInstance::isFinished() const{
 //TODO
}

bool WorkflowInstance::isCancelled() const{
 //TODO
}

void WorkflowInstance::rejectNodeName(const std::string& nodeName){
  //Probably checks the names, will have to do later
}

void WorkflowInstance::markRunning(){
 //TODO
}

const std::string& WorkflowInstance::getRejectedAt() const{
 //TODO
}
