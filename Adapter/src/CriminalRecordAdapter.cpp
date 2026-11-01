/**
 * @file CriminalRecordAdapter.cpp
 * @brief Implementation of CriminalRecordAdapter: Document → PoliceRecordsDB::lookup(idNumber) → CheckResult.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "CriminalRecordAdapter.h"
#include "Document.h"

CriminalRecordAdapter::CriminalRecordAdapter(std::shared_ptr<PoliceRecordsDB> db, const std::string& idField)
	: db(db), idField(idField) {}

CheckResult CriminalRecordAdapter::run(const Document& doc) const{
	std::string idNumber;
	if (!doc.getField(idField, idNumber))
        return CheckResult{false, "Missing field: " + idField, getName(), {}};

	//The police database searches for person with criminal record
	const CriminalRecord* record = db->lookup(idNumber);

	std::map<std::string, std::string> data;
	//doesnt exist or person doesnt have a record 
	if (!record || !record->hasRecord){
		data["hasRecord"] = "false";
		return CheckResult{true, "No criminal record", getName(), data};
	}

	//person has record, get summary of criminal record/offence
	data["hasRecord"] = "true";
    return CheckResult{false, "Criminal record found: " + record->offenceSummary, getName(), data};
}

std::string CriminalRecordAdapter::getName() const {
	return "Criminal record check";
}
