/**
 * @file ExternalCheck.h
 * @brief Target interface of the Adapter pattern: the one way TIVIDY runs any external check.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef EXTERNALCHECK_H
#define EXTERNALCHECK_H

#include <string>
#include "CheckResult.h"

class Document;

/**
 * @class ExternalCheck
 * @brief Abstract Target of the Adapter pattern.
 *
 * TIVIDY only ever talks to external systems (credit bureau, police database,
 * Home Affairs, exam board, fraud engine) through this interface. Each concrete
 * adapter translates run() into its own external system's interface and translates
 * the answer back into a CheckResult.
 *
 * ### Example Usage:
 * @code
 * std::shared_ptr<ExternalCheck> check =
 *     std::make_shared<CreditCheckAdapter>(bureau, FieldNames::ID_NUMBER, 650);
 * CheckResult result = check->run(document);
 * @endcode
 *
 * @note ExternalCheckNode holds an ExternalCheck, so any adapter can be a step in a workflow.
 * @see CreditCheckAdapter, CriminalRecordAdapter, IdentityVerificationAdapter,
 *      QualificationAdapter, FraudCheckAdapter, ExternalCheckNode
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class ExternalCheck {
public:
    /// @brief Virtual destructor so adapters are destroyed correctly through an ExternalCheck pointer.
    virtual ~ExternalCheck() {}

    /**
     * @brief Runs the check on an applicant's document.
     *
     * Reads the fields it needs from the Document, calls the external system and
     * translates the answer into a CheckResult.
     *
     * @param[in] doc The applicant's filled-in document.
     * @return The outcome. If a required field is missing or not filled,
     *         passed is false and message is "Missing field: <name>".
     */
    virtual CheckResult run(const Document& doc) const = 0;

    /**
     * @brief Returns the check's display name, used in the history and in CheckResult::checkName.
     * @return e.g. "Credit check".
     */
    virtual std::string getName() const = 0;
};

#endif
