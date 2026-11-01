/**
 * @file HomeAffairsService.cpp
 * @brief Implementation of the HomeAffairsService stub.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "HomeAffairsService.h"

#include <string>
using namespace std;

bool HomeAffairsService::verifyIdentity(const std::string& name, const std::string& surname,
                const std::string& idNumber, const std::string& dateOfBirth) const {

    const IdentityRecord* record = findRecord(idNumber);
    
    if (!record) return false;
	
    return record->name == name && record->surname == surname && record->dateOfBirth == dateOfBirth;
}

void HomeAffairsService::addRecord(const IdentityRecord& record) {
	records[record.idNumber] = record;
}

const IdentityRecord* HomeAffairsService::findRecord(const std::string& idNumber) const{
	std::map<std::string, IdentityRecord>::const_iterator it = records.find(idNumber);

    return it == records.end() ? nullptr : &it->second;
}
