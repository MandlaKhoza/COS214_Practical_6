/**
 * @file CriminalRecordAdapter.h
 * @brief Adapter: lets TIVIDY run a criminal record check through the PoliceRecordsDB.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef CRIMINALRECORDADAPTER_H
#define CRIMINALRECORDADAPTER_H

#include <memory>
#include <string>
#include "ExternalCheck.h"
#include "PoliceRecordsDB.h"

/**
 * @class CriminalRecordAdapter
 * @brief Adapter: passes if the police database has no record for the applicant.
 *
 * Translates run(Document) into PoliceRecordsDB::lookup(idNumber) and the returned
 * record pointer into a CheckResult. A person with no entry counts as clean.
 *
 * ### Example Usage:
 * @code
 * auto db = std::make_shared<PoliceRecordsDB>();
 * CriminalRecordAdapter criminal(db, FieldNames::ID_NUMBER);
 * CheckResult r = criminal.run(document);   // e.g. "No criminal record"
 * @endcode
 *
 * @see ExternalCheck, PoliceRecordsDB
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class CriminalRecordAdapter : public ExternalCheck {

	private:
		std::shared_ptr<PoliceRecordsDB> db;
		std::string idField;

	public:
		/**
		 * @brief Creates the adapter.
		 * @param[in] db      The police database to adapt.
		 * @param[in] idField Document key that holds the applicant's ID number.
		 */
		CriminalRecordAdapter(std::shared_ptr<PoliceRecordsDB> db, const std::string& idField);

		/**
		 * @brief Runs the criminal record check.
		 * @param[in] doc The applicant's document.
		 * @return The CheckResult:
		 *  - passed = true if there is no entry, or the entry has no record; data["hasRecord"] is "true" or "false".
		 *  - passed = false with "Missing field: <idField>" if the ID is not filled in.
		 *  - passed = false with "Criminal record found: <offence>" if a record exists.
		 */
		CheckResult run(const Document& doc) const;

		/**
		 * @brief Returns the check's display name.
		 * @return "Criminal record check".
		 */
		std::string getName() const;
};

#endif
