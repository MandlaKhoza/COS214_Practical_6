/**
 * @file LegacyCreditBureau.h
 * @brief Adaptee: stub of an external credit bureau with its own (incompatible) interface.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef LEGACYCREDITBUREAU_H
#define LEGACYCREDITBUREAU_H

#include <map>
#include <string>
#include "CreditRecord.h"

/**
 * @class LegacyCreditBureau
 * @brief Adaptee: a stand-in for an external credit bureau.
 *
 * Its interface (an ID number in, an int score out) does not match TIVIDY's
 * ExternalCheck, which is why CreditCheckAdapter exists. Data is loaded with
 * addRecord() during scenario setup or in tests.
 *
 * @note This is a documented stub; no real bureau is contacted.
 * @see CreditCheckAdapter, CreditRecord
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class LegacyCreditBureau {
public:
    /**
     * @brief The bureau's own interface: returns a person's credit score.
     * @param[in] idNumber The person's ID number.
     * @return The score.
     * @retval -1 The bureau has no record for this ID.
     */
    int fetchScore(const std::string& idNumber) const;

    /**
     * @brief Stores a person's record, replacing any existing record with the same ID.
     * @param[in] record The record to store.
     * @post findRecord(record.idNumber) returns this record.
     */
    void addRecord(const CreditRecord& record);

    /**
     * @brief Looks up a person's full record.
     * @param[in] idNumber The person's ID number.
     * @return A pointer to the record (owned by the bureau).
     * @retval nullptr The bureau has no record for this ID.
     */
    const CreditRecord* findRecord(const std::string& idNumber) const;

private:
    std::map<std::string, CreditRecord> records; 
};

#endif
