/**
 * @file QualificationAdapter.h
 * @brief Adapter: lets TIVIDY check exam results through the ExamBoardAPI.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef QUALIFICATIONADAPTER_H
#define QUALIFICATIONADAPTER_H

#include "ExternalCheck.h"
#include "ExamBoardAPI.h"
#include <string>
#include <memory>
#include <map>

/**
 * @class QualificationAdapter
 * @brief Adapter: passes if the exam board's marks meet every required subject mark.
 *
 * Translates run(Document) into ExamBoardAPI::getResults(candidateNo), then compares
 * each required subject with the candidate's mark. The required marks are set per
 * scenario (e.g. a programme's minimum Mathematics and English marks).
 *
 * ### Example Usage:
 * @code
 * std::map<std::string, int> required = {{"Mathematics", 50}, {"English", 50}};
 * QualificationAdapter qualification(examBoard, FieldNames::EXAM_CANDIDATE_NUMBER, required);
 * CheckResult r = qualification.run(document);
 * @endcode
 *
 * @see ExternalCheck, ExamBoardAPI
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class QualificationAdapter : public ExternalCheck {

private:
	std::shared_ptr<ExamBoardAPI> api;         ///< the adaptee
	std::string candidateField;                ///< Document key holding the candidate number
	std::map<std::string, int> requiredMarks;  ///< subject → minimum mark

public:
	/**
	 * @brief Creates the adapter.
	 * @param[in] api            The exam board to adapt.
	 * @param[in] candidateField Document key that holds the exam candidate number.
	 * @param[in] requiredMarks  subject → minimum mark needed to pass.
	 */
	QualificationAdapter(std::shared_ptr<ExamBoardAPI> api, const std::string& candidateField,
                         const std::map<std::string, int>& requiredMarks);

	/**
	 * @brief Runs the qualification check.
	 * @param[in] doc The applicant's document.
	 * @return The CheckResult:
	 *  - passed = true if every required mark is met; data holds subject → mark.
	 *  - passed = false with "Missing field: <candidateField>" if the candidate number is not filled in.
	 *  - passed = false with "No exam results found for candidate ..." if the exam board has nothing.
	 *  - passed = false with "Required marks not met: ..." listing every subject that is too low or not taken.
	 */
	CheckResult run(const Document& doc) const;

	/**
	 * @brief Returns the check's display name.
	 * @return "Qualification check".
	 */
	std::string getName() const;
};

#endif
