/**
 * @file ExternalCheckNode.cpp
 * @brief Implementation of ExternalCheckNode: the workflow step that runs an
 *        external check through the Adapter pattern.
 * @author Hafani Ramagoma
 * @date 2026-11-01
 */
#include "../inc/ExternalCheckNode.h"
#include "ui_logger.hpp"

#include <exception>
#include <string>

#include "../inc/NodeResult.h"
#include "../inc/WorkflowInstance.h"
#include "../../Adapter/inc/CheckResult.h"
#include "../../Adapter/inc/ExternalCheck.h"

/**
 * @brief Creates the node.
 * @param[in] name  The step's name, e.g. "Credit check".
 * @param[in] check The adapter to run (shared between cloned chains: adapters hold no
 *                  per-applicant data).
 */
ExternalCheckNode::ExternalCheckNode(std::string name, std::shared_ptr<ExternalCheck> check)
    : WorkflowNode(name), check(check) {}

/**
 * @brief Runs the external check on the instance's document.
 *
 * The node only sees the ExternalCheck interface (the Adapter's target), so the same
 * node works for a credit bureau, Home Affairs, an exam board or a fraud engine.
 *
 * @param[in] instance The workflow being processed.
 * @return The NodeResult:
 *         - PASSED   the check passed
 *         - REJECTED the check ran and said no (a business decision)
 *         - FAILED   the check could not run: no adapter, no document, or the external
 *                    system threw (a technical error)
 *         The name holds the step's name and the check's message, e.g.
 *         "Credit check: Credit score 590 below minimum 650".
 */
NodeResult ExternalCheckNode::process(WorkflowInstance& instance) {
    UI_LOG("[EXTERNAL] Running external check '" + getName() + "'...");
    NodeResult res;

    if (!check) {
        res.result = HandleResult::FAILED;
        res.name = getName() + ": no external check configured";
        UI_LOG("[EXTERNAL] " + res.name);
        return res;
    }

    std::shared_ptr<Document> document = instance.getDocument();
    if (!document) {
        res.result = HandleResult::FAILED;
        res.name = getName() + ": no document to check";
        UI_LOG("[EXTERNAL] " + res.name);
        return res;
    }

    try {
        CheckResult r = check->run(*document);
        res.result = r.passed ? HandleResult::PASSED : HandleResult::REJECTED;
        res.name = getName() + ": " + r.message;
        UI_LOG("[EXTERNAL] " + res.name);
    } catch (const std::exception& e) {
        res.result = HandleResult::FAILED;  // the external system broke, not a "no"
        res.name = getName() + ": check could not run (" + e.what() + ")";
        UI_LOG("[EXTERNAL] " + res.name);
    }
    return res;
}

/**
 * @brief Prototype: copies this step for a new workflow instance.
 * @return A new ExternalCheckNode with the same name and the same (shared) adapter,
 *         starting in Waiting.
 */
std::unique_ptr<WorkflowNode> ExternalCheckNode::clone() {
    return std::unique_ptr<WorkflowNode>(new ExternalCheckNode(getName(), check));
}
