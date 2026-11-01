/**
 * @file IdentityRecord.h
 * @brief One person's entry in the simulated Home Affairs register.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef IDENTITYRECORD_H
#define IDENTITYRECORD_H
#include <string>

/**
 * @brief One person's data in the simulated Home Affairs register (HomeAffairsService).
 * @see HomeAffairsService
 */
struct IdentityRecord {

	std::string idNumber;
	std::string name;
	std::string surname;
	std::string dateOfBirth; // YYYY-MM-DD
	bool verified;
};

#endif
