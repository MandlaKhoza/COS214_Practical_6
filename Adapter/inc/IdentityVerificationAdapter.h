/**
 * @file IdentityVerificationAdapter.h
 * @brief Adapter: lets TIVIDY verify an applicant's identity through the HomeAffairsService.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef IDENTITYVERIFICATIONADAPTER_H
#define IDENTITYVERIFICATIONADAPTER_H

#include "ExternalCheck.h"
#include "HomeAffairsService.h"
#include <string>
#include <memory>

/**
 * @class IdentityVerificationAdapter
 * @brief Adapter: passes if Home Affairs confirms name, surname, ID number and date of birth.
 *
 * Collects four fields from the Document and translates run(Document) into
 * HomeAffairsService::verifyIdentity(name, surname, idNumber, dob).
 *
 * ### Example Usage:
 * @code
 * auto homeAffairs = std::make_shared<HomeAffairsService>();
 * IdentityVerificationAdapter identity(homeAffairs, FieldNames::NAME, FieldNames::SURNAME,
 *                                      FieldNames::ID_NUMBER, FieldNames::DATE_OF_BIRTH);
 * CheckResult r = identity.run(document);
 * @endcode
 *
 * @see ExternalCheck, HomeAffairsService
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class IdentityVerificationAdapter : public ExternalCheck {

private:
	std::shared_ptr<HomeAffairsService> service;
	std::string nameField;
	std::string surnameField;
	std::string idField;
	std::string dobField;

public:
	/**
	 * @brief Creates the adapter.
	 * @param[in] service      The Home Affairs service to adapt.
	 * @param[in] nameField    Document key for the first name.
	 * @param[in] surnameField Document key for the surname.
	 * @param[in] idField      Document key for the ID number.
	 * @param[in] dobField     Document key for the date of birth.
	 */
	IdentityVerificationAdapter(std::shared_ptr<HomeAffairsService> service,
                                const std::string& nameField, const std::string& surnameField,
                                const std::string& idField, const std::string& dobField);

	/**
	 * @brief Runs the identity check.
	 * @param[in] doc The applicant's document.
	 * @return The CheckResult:
	 *  - passed = true if all four details match Home Affairs.
	 *  - passed = false with "Missing field: <name>" for the first of the four fields not filled in.
	 *  - passed = false with "Identity details do not match Home Affairs records" if any detail differs.
	 */
	CheckResult run(const Document& doc) const;

	/**
	 * @brief Returns the check's display name.
	 * @return "Identity verification".
	 */
	std::string getName() const;
};

#endif
