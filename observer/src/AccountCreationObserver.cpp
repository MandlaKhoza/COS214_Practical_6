#include "AccountCreationObserver.h"
#include "AccountFactory.h"
#include "AccountRegistry.h"
#include "WorkflowInstance.h"

AccountCreationObserver::AccountCreationObserver(std::shared_ptr<AccountFactory> factory, std::shared_ptr<AccountRegistry> registry) : factory(factory), registry(registry){
}

void AccountCreationObserver::onWorkflowFinished(WorkflowInstance& wi, Outcome out) {
    if(out == Outcome::Approved){
        registry->add(factory->create(wi));
    }
}
