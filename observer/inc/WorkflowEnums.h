#ifndef WORKFLOWENUMS_H
#define WORKFLOWENUMS_H

/**
 * @file WorkflowEnums.h
 * @brief Enums used by the Observer pattern.
 */

/**
 * @brief How a workflow instance finished.
 */
enum class Outcome { Approved, Rejected, Cancelled };

/**
 * @brief The current status of a workflow instance.
 */
enum class WorkflowStatus { Created, Running, Approved, Rejected, Cancelled };

#endif
