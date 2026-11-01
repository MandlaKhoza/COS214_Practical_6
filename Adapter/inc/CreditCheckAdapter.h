/**
 * @file CreditCheckAdapter.h
 * @brief Adapter: lets TIVIDY run a credit check through the LegacyCreditBureau.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef CREDITCHECKADAPTER_H
#define CREDITCHECKADAPTER_H

#include "ExternalCheck.h"
#include "LegacyCreditBureau.h"
#include <string>
#include <memory>

/**
 * @class CreditCheckAdapter
 * @brief Adapter: passes if the bureau's score for the applicant is at least minScore.
 *
 * Translates TIVIDY's run(Document) into LegacyCreditBureau::fetchScore(idNumber)
 * and the returned int back into a CheckResult. Which field holds the ID and the
 * minimum score are set per scenario, so the same class serves any workflow.
 *
 * ### Example Usage:
 * @code
 * auto bureau = std::make_shared<LegacyCreditBureau>();
 * CreditCheckAdapter credit(bureau, FieldNames::ID_NUMBER, 650);
 * CheckResult r = credit.run(document);   // e.g. "Credit score 712 meets minimum 650"
 * @endcode
 *
 * @see ExternalCheck, LegacyCreditBureau
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class CreditCheckAdapter : public ExternalCheck {
	private:
	std::shared_ptr<LegacyCreditBureau> bureau; 
	std::string idField;                     
	int minScore; 

	public:
		/**
		 * @brief Creates the adapter.
		 * @param[in] bureau   The credit bureau to adapt.
		 * @param[in] idField  Document key that holds the applicant's ID number.
		 * @param[in] minScore Lowest score that passes.
		 */
		CreditCheckAdapter(std::shared_ptr<LegacyCreditBureau> bureau, const std::string& idField, int minScore);

		/**
		 * @brief Runs the credit check.
		 * @param[in] doc The applicant's document.
		 * @return The CheckResult:
		 *  - passed = true if the score is at least minScore; data["score"] holds the score.
		 *  - passed = false with "Missing field: <idField>" if the ID is not filled in.
		 *  - passed = false with "No credit record found for ID ..." if the bureau doesn't know the person.
		 *  - passed = false with "Credit score X below minimum Y" if the score is too low.
		 */
		CheckResult run(const Document& doc) const;

		/**
		 * @brief Returns the check's display name.
		 * @return "Credit check".
		 */
		std::string getName() const;
};

#endif

