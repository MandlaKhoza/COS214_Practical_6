#include "../inc/WorkflowSubject.h"
#include "WorkflowInstance.h"
#include "ui_logger.hpp"
#include <algorithm>
#include <string>

void WorkflowSubject::attach(std::shared_ptr<WorkflowObserver> o){
    if (o) {
        observers.push_back(o);
        UI_LOG("[OBSERVER] Observer attached; total observers: " + std::to_string(observers.size()));
    }
}

void WorkflowSubject::detach(const std::shared_ptr<WorkflowObserver> o) {
    observers.erase(std::remove(observers.begin(), observers.end(), o), observers.end());
}

 void WorkflowSubject::notify(WorkflowInstance* wi, Outcome out) {
    if(wi == nullptr){
         return;
    }
    UI_LOG("[OBSERVER] Workflow '" + wi->getId() + "' notifying " +
           std::to_string(observers.size()) + " observer(s)");
    for(std::size_t i = 0; i < observers.size(); i++) {
        observers[i]->onWorkflowFinished(*wi, out);
    }
}
