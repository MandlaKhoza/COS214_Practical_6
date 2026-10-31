#include "CreditCheckAdapter.h"
#include "Document.h"

/**
 * @file CreditCheckAdapter.cpp
 * @brief Implementation of CreditCheckAdapter: Document → LegacyCreditBureau::fetchScore(idNumber) → CheckResult.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */

CreditCheckAdapter::CreditCheckAdapter(std::shared_ptr<LegacyCreditBureau> bureau,
                                       const std::string& idField, int minScore)
    : bureau(bureau), idField(idField), minScore(minScore) {}


CheckResult CreditCheckAdapter::run(const Document& doc) const {
	std::string idNumber;

	if(!doc.getField(idField, idNumber))
		return CheckResult{false, "Missing field: " + idField, getName(), {}};

	//Pass the system request to the adaptee
	int score = bureau->fetchScore(idNumber);
	if (score<0)
		//{} - the struct
		return CheckResult{false, "No credit record found for ID " + idNumber, getName(), {}};

	std::map<std::string, std::string> data;
	data["score"] = std::to_string(score);

	//credit score good
	if(score < minScore)
		return CheckResult{false,
							"Credit score " +  std::to_string(score) + " below minimum " + std::to_string(minScore), 
							getName(), data};

	return CheckResult{true,
						"Credit score " +  std::to_string(score) + " meets minimum " + std::to_string(minScore), 
						getName(), data};
}

std::string CreditCheckAdapter::getName() const {
	return "Credit check";
}
