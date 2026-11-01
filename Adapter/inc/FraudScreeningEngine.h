/**
 * @file FraudScreeningEngine.h
 * @brief Adaptee: stub of an external fraud screening engine.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef FRAUDSCREENINGENGINE_H
#define FRAUDSCREENINGENGINE_H
#include <string>
#include <map>
#include "FraudProfile.h"

/**
 * @class FraudScreeningEngine
 * @brief Adaptee: a stand-in for an external fraud screening engine.
 *
 * Its interface (an ID number and an amount in, a risk number out) does not match
 * TIVIDY's ExternalCheck, which is why FraudCheckAdapter exists.
 *
 * Risk model (deliberately simple, since this stands in for a real service):
 * - a flagged profile is always 1.0;
 * - otherwise start from the profile's riskScore, or 0.2 for an unknown person;
 * - add 0.1 for amounts above R100 000 and 0.2 for amounts above R500 000;
 * - never more than 1.0.
 *
 * @note This is a documented stub; no real engine is contacted.
 * @see FraudCheckAdapter, FraudProfile
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class FraudScreeningEngine {

	private:
		std::map<std::string, FraudProfile> records;

	public:
		/**
		 * @brief The engine's own interface: the fraud risk for a person and an amount.
		 * @param[in] idNumber The person's ID number.
		 * @param[in] amount   The amount involved (loan or refund), in rand.
		 * @return Risk between 0.0 and 1.0 (see the risk model above).
		 */
		double riskScore(const std::string& idNumber, double amount) const;

		/**
		 * @brief Stores a person's profile, replacing any existing profile with the same ID.
		 * @param[in] profile The profile to store.
		 * @post findProfile(profile.idNumber) returns this profile.
		 */
		void addProfile(const FraudProfile& profile);

		/**
		 * @brief Looks up a person's profile.
		 * @param[in] idNumber The person's ID number.
		 * @return A pointer to the profile (owned by the engine).
		 * @retval nullptr No profile for this ID.
		 */
		const FraudProfile* findProfile(const std::string& idNumber) const;
};

#endif
