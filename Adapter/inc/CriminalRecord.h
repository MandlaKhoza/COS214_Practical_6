/**
 * @file CriminalRecord.h
 * @brief One person's entry in the simulated police database.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef CRIMINALRECORD_H
#define CRIMINALRECORD_H

#include <string>

/**
 * @brief One person's data in the simulated police database (PoliceRecordsDB).
 * @see PoliceRecordsDB
 */
struct CriminalRecord {
    std::string idNumber;
    bool hasRecord;
    std::string offenceSummary;
};

#endif
