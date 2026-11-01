/**
 * @file CommandLogEntry.h
 * @brief One line in the WorkflowController's audit log.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef COMMANDLOGENTRY_H
#define COMMANDLOGENTRY_H
#include <string>

/**
 * @brief One line in the WorkflowController's audit log.
 * @see WorkflowController
 */
struct CommandLogEntry {
	std::string description;
	bool succeeded;
};

#endif
