/**
 * @file FraudCheckAdapter.h
 * @brief Adapter: lets TIVIDY run a fraud check through the FraudScreeningEngine.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef FRAUDCHECKADAPTER_H
#define FRAUDCHECKADAPTER_H

#include "ExternalCheck.h"
#include <memory>
#include "FraudScreeningEngine.h"
#include <string>

/**
 * @class FraudCheckAdapter
 * @brief Adapter: passes if the engine's risk for the applicant and amount is at most maxRisk.
 *
 * Translates run(Document) into FraudScreeningEngine::riskScore(idNumber, amount).
 * The amount field is set per scenario, so the same class checks loans
 * (FieldNames::LOAN_AMOUNT) and refunds (FieldNames::REFUND_AMOUNT).
 *
 * ### Example Usage:
 * @code
 * FraudCheckAdapter fraud(engine, FieldNames::ID_NUMBER, FieldNames::LOAN_AMOUNT, 0.5);
 * CheckResult r = fraud.run(document);   // e.g. "Fraud risk 0.30 within limit 0.50"
 * @endcode
 *
 * @see ExternalCheck, FraudScreeningEngine
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class FraudCheckAdapter : public ExternalCheck {

	private:
		std::shared_ptr<FraudScreeningEngine> engine;
		std::string idField;            
		std::string amountField;                       
		double maxRisk;                                

	public:
		/**
		 * @brief Creates the adapter.
		 * @param[in] engine      The fraud engine to adapt.
		 * @param[in] idField     Document key that holds the applicant's ID number.
		 * @param[in] amountField Document key that holds the amount (must be a number field).
		 * @param[in] maxRisk     Highest risk that still passes (0.0 to 1.0).
		 */
		FraudCheckAdapter(std::shared_ptr<FraudScreeningEngine> engine, const std::string& idField,
						const std::string& amountField, double maxRisk);

		/**
		 * @brief Runs the fraud check.
		 * @param[in] doc The applicant's document.
		 * @return The CheckResult:
		 *  - passed = true if the risk is at most maxRisk; data["risk"] holds the risk (2 decimals).
		 *  - passed = false with "Missing field: <name>" if the ID or the amount is not filled in.
		 *  - passed = false with "Fraud risk X above maximum Y" if the risk is too high.
		 */
		CheckResult run(const Document& doc) const;

		/**
		 * @brief Returns the check's display name.
		 * @return "Fraud check".
		 */
		std::string getName() const;
};

#endif
