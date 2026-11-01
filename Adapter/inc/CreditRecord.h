/**
 * @file CreditRecord.h
 * @brief One person's entry in the simulated credit bureau.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef CREDITRECORD_H
#define CREDITRECORD_H

#include <string>

/**
 * @brief One person's data in the simulated credit bureau (LegacyCreditBureau).
 * @see LegacyCreditBureau
 */

struct CreditRecord {
    std::string idNumber;
    int score;
    double outstandingDebt;
    bool hasDefault;
};

#endif
