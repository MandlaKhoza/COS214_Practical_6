/**
 * @file HomeAffairsService.h
 * @brief Adaptee: stub of the external Home Affairs identity register.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef HOMEAFFAIRSSERVICE_H
#define HOMEAFFAIRSSERVICE_H

#include <map>
#include <string>
#include "IdentityRecord.h"

/**
 * @class HomeAffairsService
 * @brief Adaptee: a stand-in for the Home Affairs identity register.
 *
 * Its interface (four separate strings in, a bool out) does not match TIVIDY's
 * ExternalCheck, which is why IdentityVerificationAdapter exists.
 *
 * @note This is a documented stub; no real service is contacted.
 * @see IdentityVerificationAdapter, IdentityRecord
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class HomeAffairsService {
public:
    /**
     * @brief The service's own interface: confirms a person's identity.
     * @param[in] name        First name.
     * @param[in] surname     Surname.
     * @param[in] idNumber    ID number.
     * @param[in] dateOfBirth Date of birth, YYYY-MM-DD.
     * @retval true  A record with this ID exists and name, surname and date of birth all match.
     * @retval false No record, or any detail differs (comparison is exact).
     */
    bool verifyIdentity(const std::string& name, const std::string& surname,
                        const std::string& idNumber, const std::string& dateOfBirth) const;

    /**
     * @brief Stores a person's record, replacing any existing record with the same ID.
     * @param[in] record The record to store.
     * @post findRecord(record.idNumber) returns this record.
     */
    void addRecord(const IdentityRecord& record);

    /**
     * @brief Looks up a person's record.
     * @param[in] idNumber The person's ID number.
     * @return A pointer to the record (owned by the service).
     * @retval nullptr No record for this ID.
     */
    const IdentityRecord* findRecord(const std::string& idNumber) const;

private:
    std::map<std::string, IdentityRecord> records;  ///< ID number → record
};

#endif
