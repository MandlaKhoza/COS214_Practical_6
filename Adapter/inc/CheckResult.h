/**
 * @file CheckResult.h
 * @brief The result every external check returns.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef CHECKRESULT_H
#define CHECKRESULT_H

#include <map>
#include <string>

/**
 * @brief What an external check returns: TIVIDY's side of the adapter translation.
 *
 * Fill it in member order, e.g.
 * @code
 * return CheckResult{false, "Missing field: idNumber", getName(), {}};
 * @endcode
 * @see ExternalCheck
 */
struct CheckResult {
	bool passed;
	std::string message;
	std::string checkName;
	std::map<std::string, std::string> data;
};

#endif
