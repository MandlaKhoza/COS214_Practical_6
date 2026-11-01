/**
 * @file FraudScreeningEngine.cpp
 * @brief Implementation of the FraudScreeningEngine stub (risk model described in the header).
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "FraudScreeningEngine.h"

namespace {
	const double UNKNOWN_PERSON_RISK = 0.2;
}

double FraudScreeningEngine::riskScore(const std::string& idNumber, double amount) const{
	const FraudProfile* profile = findProfile(idNumber);
	
	if (profile && profile->flagged) return 1.0;

	double risk = profile ? profile->riskScore : UNKNOWN_PERSON_RISK;
	if (amount>500000.0) risk += 0.2;

	else if (amount > 100000.0) risk += 0.1;

	return risk > 1.0 ? 1.0 : risk;

}

void FraudScreeningEngine::addProfile(const FraudProfile& profile) {
	records[profile.idNumber] = profile;
}

const FraudProfile* FraudScreeningEngine::findProfile(const std::string& idNumber) const{
	std::map<std::string, FraudProfile>::const_iterator it = records.find(idNumber);
    return it == records.end() ? nullptr : &it->second;
}
