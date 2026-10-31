/**
 * @file AppealCommand.cpp
 * @brief Implementation of AppealCommand.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "AppealCommand.h"

#include "AnswerDocumentBuilder.h"
#include "ResetWorkflowCommand.h"
#include "WorkflowInstance.h"

AppealCommand::AppealCommand(WorkflowInstance* instance, const std::map<std::string, std::string>& updatedAnswers)
    : instance(instance), updatedAnswers(updatedAnswers) {}

bool AppealCommand::execute(){
    //refusals , so a refused appeal changes nothing
    if (!instance) return false;
    if (instance->getStatus() != WorkflowStatus::Rejected) return false;  // only rejected applications
    if (instance->hasAppealed()) return false;                            // one appeal per application
    if (updatedAnswers.empty()) return false;                             // nothing changed: same result again


    // 1. Apply the corrections. Reuses the Builder's cross-reference step: only fields
    //    already on the form are updated; anything else in the map is ignored.
    AnswerDocumentBuilder updater(&instance->editDocument(), updatedAnswers);
    updater.setMapValues();

    // 2. Reuse the macro command to put every node back to Waiting.
    ResetWorkflowCommand reset(instance);
    if (!reset.execute()) return false;

    // 3. Record it, then run every check again on the new data.
    instance->markAppealed();

    instance->addHistory("Workflow", "Appeal: resubmitted with updated details");

    instance->run();
    
    return true;
}



std::string AppealCommand::describe() const {
    return "Appeal workflow " + (instance ? instance->getId() : std::string("(none)"));
}