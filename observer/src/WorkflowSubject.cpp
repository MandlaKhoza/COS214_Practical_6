#include "WorkflowSubject.h"
#include <algorithm>

void WorkflowSubject::attach(std::shared_ptr<WorkflowObserver> o){
    if (o) {
        observers.push_back(o);
    }
}

void WorkflowSubject::detach(const std::shared_ptr<WorkflowObserver> o) {
    observers.erase(std::remove(observers.begin(), observers.end(), o), observers.end());
}

 void WorkflowSubject::notify(WorkflowInstance* wi, Outcome out) {
    if(wi == nullptr){
         return;
    }
    for(std::size_t i = 0; i < observers.size(); i++) {
        observers[i]->onWorkflowFinished(*wi, out);
    }
}
