/**
 * @file FraudCheckAdapter.cpp
 * @brief Implementation of FraudCheckAdapter: Document (ID + amount) → FraudScreeningEngine::riskScore → CheckResult.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#include "FraudCheckAdapter.h"

#include <iomanip>
#include <sstream>
#include "Document.h"

namespace {
/**
 * @brief Formats a number with two decimals, for messages and data.
 * @param[in] value The number to format, e.g. 0.3.
 * @return The text, e.g. "0.30".
 */
std::string twoDecimalsPlaces(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;
    return out.str();
}
}

FraudCheckAdapter::FraudCheckAdapter(std::shared_ptr<FraudScreeningEngine> engine, const std::string& idField,
                                    const std::string& amountField, double maxRisk)
    : engine(engine), idField(idField), amountField(amountField), maxRisk(maxRisk) {}

CheckResult FraudCheckAdapter::run(const Document& doc) const{
	std::string idNumber;
	if (!doc.getField(idField, idNumber))
        return CheckResult{false, "Missing field: " + idField, getName(), {}};

	double amount = 0.0;
	if (!doc.getNumber(amountField, amount))
        return CheckResult{false, "Missing field: " + amountField, getName(), {}};

	double risk = engine->riskScore(idNumber, amount);
	std::map<std::string, std::string> data;
	data["risk"] = twoDecimalsPlaces(risk);

	if (risk >maxRisk)
		return CheckResult{false, "Fraud risk " + twoDecimalsPlaces(risk) + " above maximum " + twoDecimalsPlaces(maxRisk), getName(), data};

	return CheckResult{true, "Fraud risk " + twoDecimalsPlaces(risk) + " within limit " + twoDecimalsPlaces(maxRisk), getName(), data};
}

std::string FraudCheckAdapter::getName() const{
	return "Fraud check";
}
