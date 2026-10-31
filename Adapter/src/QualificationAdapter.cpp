/**
 * @file QualificationAdapter.cpp
 * @brief Implementation of QualificationAdapter: Document → ExamBoardAPI::getResults(candidateNo) → compare with required marks.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */


#include "QualificationAdapter.h"

#include "Document.h"

QualificationAdapter::QualificationAdapter(std::shared_ptr<ExamBoardAPI> api, const std::string& candidateField,
                                           const std::map<std::string, int>& requiredMarks)
    : api(api), candidateField(candidateField), requiredMarks(requiredMarks) {}

CheckResult QualificationAdapter::run(const Document& doc) const{
	std::string candidateNo;

	if(!doc.getField(candidateField, candidateNo))
		return CheckResult{false, "Missing field: " + candidateField, getName(), {}};
	
		//get results of candidate
	std::map<std::string, int> results = api->getResults(candidateNo);
	if (results.empty())
		return CheckResult{false, "No exam results found for candidate " + candidateNo, getName(), {}};

	std::map<std::string, std::string> data;
	std::string shortfalls;

	for(std::map<std::string, int>::const_iterator req = requiredMarks.begin(); req != requiredMarks.end(); ++req){
		std::map<std::string, int>::const_iterator got = results.find(req->first);
        if (got == results.end()) {
            shortfalls += (shortfalls.empty() ? "" : ", ") + req->first + " not taken";
            continue;
        }
        data[req->first] = std::to_string(got->second);
        if (got->second < req->second)
            shortfalls += (shortfalls.empty() ? "" : ", ") + req->first + " " + std::to_string(got->second) +
                          " < " + std::to_string(req->second);
	}
	if (!shortfalls.empty())
        return CheckResult{false, "Required marks not met: " + shortfalls, getName(), data};

    return CheckResult{true, "All required marks met", getName(), data};
}

std::string QualificationAdapter::getName() const{
	return "Qualification check";
}
