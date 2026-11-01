#ifndef WORKFLOWENUMS_H
#define WORKFLOWENUMS_H

/**
 * @file WorkflowEnums.h
 * @brief Enums used by the Observer pattern.
 */

/**
 * @brief How a workflow instance finished.
 */
enum class Outcome { Approved, Rejected, Cancelled, Failed };

#endif
