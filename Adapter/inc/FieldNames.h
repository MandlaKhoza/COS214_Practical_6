#ifndef FIELDNAMES_H
#define FIELDNAMES_H

#include <string>

/**
 * @brief One agreed key for every document field («utility», not a design pattern).
 *
 * Builders, nodes, adapters, observers and factories all use these constants
 * instead of typing strings, so a typo becomes a compile error.
 * A namespace is used because a static const std::string inside a class needs
 * a separate .cpp definition in C++11.
 */
namespace FieldNames {
    // Personal (every scenario)
    const std::string NAME = "name";
    const std::string SURNAME = "surname";
    const std::string ID_NUMBER = "idNumber";
    const std::string DATE_OF_BIRTH = "dateOfBirth";
    const std::string EMAIL = "email";
    const std::string PHONE = "phone";

    // University
    const std::string PROGRAMME = "programme";
    const std::string APS = "aps";
    const std::string EXAM_CANDIDATE_NUMBER = "examCandidateNumber";
    const std::string PREVIOUS_DEGREE = "previousDegree";
    const std::string ID_COPY = "idCopy";
    const std::string CERTIFIED_RESULTS = "certifiedResults";

    // Loan
    const std::string LOAN_AMOUNT = "loanAmount";
    const std::string TERM_MONTHS = "termMonths";
    const std::string MONTHLY_INCOME = "monthlyIncome";
    const std::string MONTHLY_EXPENSES = "monthlyExpenses";
    const std::string EMPLOYER = "employer";
    const std::string PAYSLIPS = "payslips";
    const std::string BANK_STATEMENTS = "bankStatements";

    // Refund
    const std::string ORDER_NUMBER = "orderNumber";
    const std::string PURCHASE_DATE = "purchaseDate";
    const std::string REFUND_AMOUNT = "refundAmount";
    const std::string REFUND_REASON = "refundReason";
    const std::string PROOF_OF_PURCHASE = "proofOfPurchase";
}

#endif
