/**
 * @file IdentityVerificationAdapter.cpp
 * @brief Implementation of IdentityVerificationAdapter: Document (four fields) → HomeAffairsService::verifyIdentity → CheckResult.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "IdentityVerificationAdapter.h"
#include "Document.h"

IdentityVerificationAdapter::IdentityVerificationAdapter(std::shared_ptr<HomeAffairsService> service,
        		const std::string& nameField, const std::string& surnameField, const std::string& idField,
                const std::string& dobField)
    			: service(service), nameField(nameField), surnameField(surnameField), idField(idField), dobField(dobField) {}

CheckResult IdentityVerificationAdapter::run(const Document& doc) const{
	std::string name, surname, idNumber, dob;

    if (!doc.getField(nameField, name))
        return CheckResult{false, "Missing field: " + nameField, getName(), {}};

    if (!doc.getField(surnameField, surname))
        return CheckResult{false, "Missing field: " + surnameField, getName(), {}};

    if (!doc.getField(idField, idNumber))
        return CheckResult{false, "Missing field: " + idField, getName(), {}};

    if (!doc.getField(dobField, dob))
        return CheckResult{false, "Missing field: " + dobField, getName(), {}};

    if (service->verifyIdentity(name, surname, idNumber, dob))
        return CheckResult{true, "Identity confirmed by Home Affairs", getName(), {}};

    return CheckResult{false, "Identity details do not match Home Affairs records", getName(), {}};
}

std::string IdentityVerificationAdapter::getName() const {
	return "Identity verification";
}
