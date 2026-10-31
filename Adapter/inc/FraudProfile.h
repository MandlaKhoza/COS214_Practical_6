/**
 * @file FraudProfile.h
 * @brief One person's entry in the simulated fraud screening engine.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef FRAUDPROFILE_H
#define FRAUDPROFILE_H

#include <string>

/**
 * @brief One person's profile in the simulated fraud screening engine (FraudScreeningEngine).
 * @see FraudScreeningEngine
 */
struct FraudProfile {
    std::string idNumber;
    double riskScore;  ///< 0.0 (no risk) to 1.0
    bool flagged;
    std::string reason;
};

#endif
