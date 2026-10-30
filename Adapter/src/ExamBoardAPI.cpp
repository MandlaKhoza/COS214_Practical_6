/**
 * @file ExamBoardAPI.cpp
 * @brief Implementation of the ExamBoardAPI stub.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "ExamBoardAPI.h"

std::map<std::string, int> ExamBoardAPI::getResults(const std::string& candidateNo) const{
	const ExamRecord* record = findRecord(candidateNo);

	return record ? record->results : std::map<std::string, int>();
}

void ExamBoardAPI::addRecord(const ExamRecord& record) {
	records[record.candidateNo] = record;
}

const ExamRecord* ExamBoardAPI::findRecord(const std::string& candidateNo) const {
	std::map<std::string, ExamRecord>::const_iterator it = records.find(candidateNo);

	return it == records.end() ? nullptr : &it->second;
}
