/**
 * @file PoliceRecordsDB.h
 * @brief Adaptee: stub of the external police records database.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef POLICERECORDSDB_H
#define POLICERECORDSDB_H
#include <string>
#include <map>
#include "CriminalRecord.h"

/**
 * @class PoliceRecordsDB
 * @brief Adaptee: a stand-in for the police records database.
 *
 * Its interface (an ID number in, a record pointer out) does not match TIVIDY's
 * ExternalCheck, which is why CriminalRecordAdapter exists.
 *
 * @note This is a documented stub; no real database is contacted.
 * @see CriminalRecordAdapter, CriminalRecord
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class PoliceRecordsDB {

	private:
		std::map<std::string, CriminalRecord> records;  ///< ID number → record

	public:
		/**
		 * @brief The database's own interface: looks up a person.
		 * @param[in] idNumber The person's ID number.
		 * @return A pointer to the record (owned by the database).
		 * @retval nullptr No entry for this ID; treated as a clean record.
		 */
		const CriminalRecord* lookup(const std::string& idNumber) const;

		/**
		 * @brief Stores a person's record, replacing any existing record with the same ID.
		 * @param[in] record The record to store.
		 * @post lookup(record.idNumber) returns this record.
		 */
		void addRecord(const CriminalRecord& record);
};

#endif
