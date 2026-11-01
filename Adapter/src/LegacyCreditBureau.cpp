/**
 * @file LegacyCreditBureau.cpp
 * @brief Implementation of the LegacyCreditBureau stub.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "LegacyCreditBureau.h"

int LegacyCreditBureau::fetchScore(const std::string& idNumber) const {
	const CreditRecord* record = findRecord(idNumber);
	//if person is unknown
	return record ? record->score : -1;
}

void LegacyCreditBureau::addRecord(const CreditRecord& record) {
	records[record.idNumber] = record;
}

const CreditRecord* LegacyCreditBureau::findRecord(const std::string& idNumber) const {
	//look through record map for name
	std::map<std::string, CreditRecord>::const_iterator it = records.find(idNumber);
	//get record attached to id or return null
    return it == records.end() ? nullptr : &it->second;
}
