#include "../inc/WorkflowInstance.h"

namespace {
/// @brief Readable name of a status, for the history.
std::string statusName(WorkflowStatus s) {
    switch (s) {
        case WorkflowStatus::Created:   return "Created";
        case WorkflowStatus::Running:   return "Running";
        case WorkflowStatus::Approved:  return "Approved";
        case WorkflowStatus::Rejected:  return "Rejected";
        case WorkflowStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}
}

WorkflowInstance::WorkflowInstance(const std::string& id, const std::string& type, 
  std::shared_ptr<Document> document)
    : id(id), document(document), status(WorkflowStatus::Created), current(nullptr), type(type) {}

const std::string& WorkflowInstance::getId() const {
    return this->id;
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

void WorkflowInstance::run() {
    if(!chain){
        throw std::logic_error("Cannot run workflow: no chain configured");
    }
    markRunning();
    chain->handle(*this);
}

void WorkflowInstance::resume() {
    if(!current || current->getStateName() != "Passed"){
        throw std::logic_error("Cannot resume: current node has not been approved");
    }

    WorkflowNode* next = current->getNext();
    if(!next){
        complete();
        return;
    }

    markRunning();
    next->handle(*this);
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
 object.stateName = statusName(this->status);
 object.message = message;

 this->history.push_back(object);
}

const std::vector<HistoryEntry>& WorkflowInstance::getHistory() const{
    return this->history;
}

void WorkflowInstance::complete(){
  this->status = WorkflowStatus::Approved;
  notify(this, Outcome::Approved);   // Observer: tell everyone the workflow finished
}

void WorkflowInstance::cancel(){
  this->status = WorkflowStatus::Cancelled;
  notify(this, Outcome::Cancelled);
}

bool WorkflowInstance::isFinished() const{
  if(this->status == WorkflowStatus::Approved || this->status == WorkflowStatus::Rejected){
    return true;
  } 
  return false;
}

bool WorkflowInstance::isCancelled() const{
  if(this->status == WorkflowStatus::Cancelled){
    return true;
  }
  return false;
}

void WorkflowInstance::reject(const std::string& nodeName){
  this->status = WorkflowStatus::Rejected;
  this->rejectedAt = nodeName;
  notify(this, Outcome::Rejected);
}

void WorkflowInstance::markRunning(){
  this->status = WorkflowStatus::Running;
}

const std::string& WorkflowInstance::getRejectedAt() const{
  return this->rejectedAt;
}


const std::string& WorkflowInstance::getType() const {
    return this->type;
}