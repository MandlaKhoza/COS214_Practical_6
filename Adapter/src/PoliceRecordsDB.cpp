/**
 * @file PoliceRecordsDB.cpp
 * @brief Implementation of the PoliceRecordsDB stub.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "PoliceRecordsDB.h"

#include <string>
using namespace std;

const CriminalRecord* PoliceRecordsDB::lookup(const std::string& idNumber) const {
	std::map<std::string, CriminalRecord>::const_iterator it = records.find(idNumber);
	
	return it == records.end() ? nullptr : &it->second;
}

void PoliceRecordsDB::addRecord(const CriminalRecord& record) {
	records[record.idNumber] = record;
}
