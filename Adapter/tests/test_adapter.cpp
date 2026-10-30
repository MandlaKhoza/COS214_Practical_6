/**
 * @file test_adapter.cpp
 * @brief Unit tests for the Adapter pattern: the five adaptees (external-system stubs)
 *        and the five adapters.
 *
 * Layout follows the COS 214 testing lecture: one TEST_CASE per unit, one SUBCASE per
 * scenario, each written as Arrange / Act / Assert, covering positive, negative,
 * edge-case and error behaviour. The Document dependency is mocked by
 * tests/support/DocumentStub.cpp.
 *
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "CreditCheckAdapter.h"
#include "CriminalRecordAdapter.h"
#include "Document.h"
#include "FieldNames.h"
#include "FraudCheckAdapter.h"
#include "IdentityVerificationAdapter.h"
#include "QualificationAdapter.h"

namespace {

const std::string ID = "0102155009087";        ///< a test applicant's ID number
const std::string OTHER_ID = "9001015800085";  ///< a second test applicant
const std::string UNKNOWN_ID = "0000000000000";///< an ID no external system knows

/**
 * @brief Builds a Document with the given text and number fields filled in.
 * @param[in] text    Text fields: key → value.
 * @param[in] numbers Number fields: key → value.
 * @return The filled Document.
 */
Document makeDoc(const std::map<std::string, std::string>& text,
                 const std::map<std::string, double>& numbers = std::map<std::string, double>()) {
    Document doc;
    for (std::map<std::string, std::string>::const_iterator i = text.begin(); i != text.end(); ++i) {
        doc.addTextField(i->first);
        doc.setText(i->first, i->second);
    }
    for (std::map<std::string, double>::const_iterator i = numbers.begin(); i != numbers.end(); ++i) {
        doc.addNumberField(i->first);
        doc.setNumber(i->first, i->second);
    }
    return doc;
}

}  // namespace

// =====================================================================================
//                                       ADAPTEES
// =====================================================================================

TEST_CASE("LegacyCreditBureau functions as expected") {
    // Arrange (shared by every subcase; doctest re-runs this for each one)
    LegacyCreditBureau bureau;
    bureau.addRecord(CreditRecord{ID, 712, 15000.0, false});

    SUBCASE("Positive: returns the score of a person it knows") {
        // Act
        int score = bureau.fetchScore(ID);
        // Assert
        CHECK(score == 712);
    }

    SUBCASE("Positive: findRecord returns the full record") {
        const CreditRecord* record = bureau.findRecord(ID);
        REQUIRE(record != nullptr);
        CHECK(record->outstandingDebt == doctest::Approx(15000.0));
        CHECK_FALSE(record->hasDefault);
    }

    SUBCASE("Negative: an unknown person gives -1") {
        CHECK(bureau.fetchScore(UNKNOWN_ID) == -1);
    }

    SUBCASE("Negative: findRecord returns nullptr for an unknown ID") {
        CHECK(bureau.findRecord(UNKNOWN_ID) == nullptr);
    }

    SUBCASE("Edge: adding the same ID twice keeps the newer record") {
        bureau.addRecord(CreditRecord{ID, 540, 90000.0, true});
        CHECK(bureau.fetchScore(ID) == 540);
    }

    SUBCASE("Edge: a score of 0 is a real score, not 'unknown'") {
        bureau.addRecord(CreditRecord{OTHER_ID, 0, 0.0, false});
        CHECK(bureau.fetchScore(OTHER_ID) == 0);
    }

    SUBCASE("Errors: looking up an unknown person never throws") {
        CHECK_NOTHROW(bureau.fetchScore(UNKNOWN_ID));
    }
}

TEST_CASE("PoliceRecordsDB functions as expected") {
    PoliceRecordsDB db;
    db.addRecord(CriminalRecord{ID, true, "Fraud (2019)"});

    SUBCASE("Positive: lookup returns a stored record") {
        const CriminalRecord* record = db.lookup(ID);
        REQUIRE(record != nullptr);
        CHECK(record->hasRecord);
        CHECK(record->offenceSummary == "Fraud (2019)");
    }

    SUBCASE("Negative: lookup returns nullptr for an unknown ID") {
        CHECK(db.lookup(UNKNOWN_ID) == nullptr);
    }

    SUBCASE("Edge: adding the same ID twice keeps the newer record") {
        db.addRecord(CriminalRecord{ID, false, ""});
        REQUIRE(db.lookup(ID) != nullptr);
        CHECK_FALSE(db.lookup(ID)->hasRecord);
    }

    SUBCASE("Errors: looking up an unknown person never throws") {
        CHECK_NOTHROW(db.lookup(UNKNOWN_ID));
    }
}

TEST_CASE("HomeAffairsService functions as expected") {
    HomeAffairsService service;
    service.addRecord(IdentityRecord{ID, "Hafs", "Ramagoma", "2001-02-15", true});

    SUBCASE("Positive: matching details are verified") {
        CHECK(service.verifyIdentity("Hafs", "Ramagoma", ID, "2001-02-15"));
    }

    SUBCASE("Negative: a different surname is not verified") {
        CHECK_FALSE(service.verifyIdentity("Hafs", "Smith", ID, "2001-02-15"));
    }

    SUBCASE("Negative: a different date of birth is not verified") {
        CHECK_FALSE(service.verifyIdentity("Hafs", "Ramagoma", ID, "2001-02-16"));
    }

    SUBCASE("Negative: an unknown ID is not verified") {
        CHECK_FALSE(service.verifyIdentity("Hafs", "Ramagoma", UNKNOWN_ID, "2001-02-15"));
    }

    SUBCASE("Edge: the comparison is case-sensitive") {
        CHECK_FALSE(service.verifyIdentity("hafs", "Ramagoma", ID, "2001-02-15"));
    }

    SUBCASE("Positive: findRecord returns the stored record") {
        const IdentityRecord* record = service.findRecord(ID);
        REQUIRE(record != nullptr);
        CHECK(record->surname == "Ramagoma");
    }
}

TEST_CASE("ExamBoardAPI functions as expected") {
    ExamBoardAPI api;
    api.addRecord(ExamRecord{"C1001", "Hafs Ramagoma", {{"Mathematics", 78}, {"English", 65}}});

    SUBCASE("Positive: returns a candidate's marks") {
        std::map<std::string, int> results = api.getResults("C1001");
        CHECK(results.size() == 2);
        CHECK(results["Mathematics"] == 78);
    }

    SUBCASE("Negative: an unknown candidate gives no results") {
        CHECK(api.getResults("C9999").empty());
        CHECK(api.findRecord("C9999") == nullptr);
    }

    SUBCASE("Edge: adding the same candidate twice keeps the newer record") {
        api.addRecord(ExamRecord{"C1001", "Hafs Ramagoma", {{"Mathematics", 40}}});
        std::map<std::string, int> results = api.getResults("C1001");
        CHECK(results.size() == 1);
        CHECK(results["Mathematics"] == 40);
    }
}

TEST_CASE("FraudScreeningEngine functions as expected") {
    FraudScreeningEngine engine;
    engine.addProfile(FraudProfile{ID, 0.1, false, ""});
    engine.addProfile(FraudProfile{OTHER_ID, 0.3, true, "Identity theft report"});

    SUBCASE("Positive: a known person's base risk is used for small amounts") {
        CHECK(engine.riskScore(ID, 50000) == doctest::Approx(0.1));
    }

    SUBCASE("Positive: an unknown person starts at 0.2") {
        CHECK(engine.riskScore(UNKNOWN_ID, 50000) == doctest::Approx(0.2));
    }

    SUBCASE("Negative: a flagged person is always 1.0") {
        CHECK(engine.riskScore(OTHER_ID, 10) == doctest::Approx(1.0));
    }

    SUBCASE("Edge: exactly R100 000 adds nothing; just above adds 0.1") {
        CHECK(engine.riskScore(ID, 100000.0) == doctest::Approx(0.1));
        CHECK(engine.riskScore(ID, 100000.01) == doctest::Approx(0.2));
    }

    SUBCASE("Edge: above R500 000 adds 0.2") {
        CHECK(engine.riskScore(ID, 900000.0) == doctest::Approx(0.3));
    }

    SUBCASE("Edge: risk is capped at 1.0") {
        engine.addProfile(FraudProfile{ID, 0.95, false, ""});
        CHECK(engine.riskScore(ID, 900000.0) == doctest::Approx(1.0));
    }
}

// =====================================================================================
//                                        ADAPTERS
// =====================================================================================

TEST_CASE("CreditCheckAdapter functions as expected") {
    // Arrange: the mock external system and the adapter
    std::shared_ptr<LegacyCreditBureau> bureau = std::make_shared<LegacyCreditBureau>();
    bureau->addRecord(CreditRecord{ID, 712, 15000.0, false});
    bureau->addRecord(CreditRecord{OTHER_ID, 590, 80000.0, true});
    CreditCheckAdapter adapter(bureau, FieldNames::ID_NUMBER, 650);

    SUBCASE("Positive: passes when the score meets the minimum") {
        // Act
        CheckResult r = adapter.run(makeDoc({{FieldNames::ID_NUMBER, ID}}));
        // Assert
        CHECK(r.passed);
        CHECK(r.checkName == "Credit check");
        CHECK(r.message == "Credit score 712 meets minimum 650");
        CHECK(r.data["score"] == "712");
    }

    SUBCASE("Negative: rejects a score below the minimum") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::ID_NUMBER, OTHER_ID}}));
        CHECK_FALSE(r.passed);
        CHECK(r.message == "Credit score 590 below minimum 650");
    }

    SUBCASE("Negative: fails when the bureau does not know the person") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::ID_NUMBER, UNKNOWN_ID}}));
        CHECK_FALSE(r.passed);
        CHECK(r.message == "No credit record found for ID " + UNKNOWN_ID);
    }

    SUBCASE("Edge: a score exactly at the minimum passes") {
        CreditCheckAdapter strict(bureau, FieldNames::ID_NUMBER, 712);
        CHECK(strict.run(makeDoc({{FieldNames::ID_NUMBER, ID}})).passed);
    }

    SUBCASE("Errors: an ID on the form but left blank gives 'Missing field'") {
        Document doc;
        doc.addTextField(FieldNames::ID_NUMBER);  // on the form, never filled in
        CheckResult r = adapter.run(doc);
        CHECK_FALSE(r.passed);
        CHECK(r.message == "Missing field: idNumber");
    }

    SUBCASE("Errors: an empty Document gives 'Missing field' and does not throw") {
        CHECK_NOTHROW(adapter.run(Document()));
        CHECK(adapter.run(Document()).message == "Missing field: idNumber");
    }
}

TEST_CASE("CriminalRecordAdapter functions as expected") {
    std::shared_ptr<PoliceRecordsDB> db = std::make_shared<PoliceRecordsDB>();
    db->addRecord(CriminalRecord{ID, true, "Fraud (2019)"});
    db->addRecord(CriminalRecord{OTHER_ID, false, ""});
    CriminalRecordAdapter adapter(db, FieldNames::ID_NUMBER);

    SUBCASE("Positive: a person with no entry is clean") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::ID_NUMBER, UNKNOWN_ID}}));
        CHECK(r.passed);
        CHECK(r.message == "No criminal record");
        CHECK(r.data["hasRecord"] == "false");
    }

    SUBCASE("Positive: an entry that says 'no record' is clean") {
        CHECK(adapter.run(makeDoc({{FieldNames::ID_NUMBER, OTHER_ID}})).passed);
    }

    SUBCASE("Negative: rejects a person with a record, naming the offence") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::ID_NUMBER, ID}}));
        CHECK_FALSE(r.passed);
        CHECK(r.message == "Criminal record found: Fraud (2019)");
        CHECK(r.data["hasRecord"] == "true");
    }

    SUBCASE("Errors: a missing ID gives 'Missing field'") {
        CHECK(adapter.run(Document()).message == "Missing field: idNumber");
    }
}

TEST_CASE("IdentityVerificationAdapter functions as expected") {
    std::shared_ptr<HomeAffairsService> service = std::make_shared<HomeAffairsService>();
    service->addRecord(IdentityRecord{ID, "Hafs", "Ramagoma", "2001-02-15", true});
    IdentityVerificationAdapter adapter(service, FieldNames::NAME, FieldNames::SURNAME,
                                        FieldNames::ID_NUMBER, FieldNames::DATE_OF_BIRTH);

    SUBCASE("Positive: passes when all four details match") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::NAME, "Hafs"}, {FieldNames::SURNAME, "Ramagoma"},
                                             {FieldNames::ID_NUMBER, ID}, {FieldNames::DATE_OF_BIRTH, "2001-02-15"}}));
        CHECK(r.passed);
        CHECK(r.message == "Identity confirmed by Home Affairs");
    }

    SUBCASE("Negative: rejects when one detail differs") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::NAME, "Hafs"}, {FieldNames::SURNAME, "Smith"},
                                             {FieldNames::ID_NUMBER, ID}, {FieldNames::DATE_OF_BIRTH, "2001-02-15"}}));
        CHECK_FALSE(r.passed);
        CHECK(r.message == "Identity details do not match Home Affairs records");
    }

    SUBCASE("Errors: names the first missing field (surname)") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::NAME, "Hafs"}, {FieldNames::ID_NUMBER, ID},
                                             {FieldNames::DATE_OF_BIRTH, "2001-02-15"}}));
        CHECK(r.message == "Missing field: surname");
    }

    SUBCASE("Edge: with every field missing, the name is reported first") {
        CHECK(adapter.run(Document()).message == "Missing field: name");
    }
}

TEST_CASE("QualificationAdapter functions as expected") {
    std::shared_ptr<ExamBoardAPI> api = std::make_shared<ExamBoardAPI>();
    api->addRecord(ExamRecord{"C1001", "Hafs Ramagoma", {{"Mathematics", 78}, {"English", 65}}});
    api->addRecord(ExamRecord{"C1002", "Thabo Mokoena", {{"Mathematics", 45}, {"English", 70}}});
    std::map<std::string, int> required;
    required["Mathematics"] = 50;
    required["English"] = 50;
    QualificationAdapter adapter(api, FieldNames::EXAM_CANDIDATE_NUMBER, required);

    SUBCASE("Positive: passes when every required mark is met") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C1001"}}));
        CHECK(r.passed);
        CHECK(r.data["Mathematics"] == "78");
    }

    SUBCASE("Negative: rejects a mark below the minimum, naming it") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C1002"}}));
        CHECK_FALSE(r.passed);
        CHECK(r.message == "Required marks not met: Mathematics 45 < 50");
    }

    SUBCASE("Negative: rejects a required subject that was not taken") {
        std::map<std::string, int> withScience = required;
        withScience["Physical Sciences"] = 50;
        QualificationAdapter science(api, FieldNames::EXAM_CANDIDATE_NUMBER, withScience);
        CHECK(science.run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C1001"}})).message ==
              "Required marks not met: Physical Sciences not taken");
    }

    SUBCASE("Negative: fails when the exam board has no results") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C9999"}}));
        CHECK_FALSE(r.passed);
        CHECK(r.message == "No exam results found for candidate C9999");
    }

    SUBCASE("Edge: a mark exactly at the minimum passes") {
        std::map<std::string, int> exact;
        exact["English"] = 65;
        QualificationAdapter exactAdapter(api, FieldNames::EXAM_CANDIDATE_NUMBER, exact);
        CHECK(exactAdapter.run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C1001"}})).passed);
    }

    SUBCASE("Errors: a missing candidate number gives 'Missing field'") {
        CHECK(adapter.run(Document()).message == "Missing field: examCandidateNumber");
    }
}

TEST_CASE("FraudCheckAdapter functions as expected") {
    std::shared_ptr<FraudScreeningEngine> engine = std::make_shared<FraudScreeningEngine>();
    engine->addProfile(FraudProfile{ID, 0.1, false, ""});
    engine->addProfile(FraudProfile{OTHER_ID, 0.3, true, "Identity theft report"});
    FraudCheckAdapter adapter(engine, FieldNames::ID_NUMBER, FieldNames::LOAN_AMOUNT, 0.5);

    SUBCASE("Positive: passes a low-risk person and a small amount") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::ID_NUMBER, ID}}, {{FieldNames::LOAN_AMOUNT, 50000}}));
        CHECK(r.passed);
        CHECK(r.data["risk"] == "0.10");
        CHECK(r.message == "Fraud risk 0.10 within limit 0.50");
    }

    SUBCASE("Negative: rejects a flagged person whatever the amount") {
        CheckResult r = adapter.run(makeDoc({{FieldNames::ID_NUMBER, OTHER_ID}}, {{FieldNames::LOAN_AMOUNT, 1000}}));
        CHECK_FALSE(r.passed);
        CHECK(r.message == "Fraud risk 1.00 above maximum 0.50");
    }

    SUBCASE("Edge: a risk exactly at the maximum passes") {
        FraudCheckAdapter strict(engine, FieldNames::ID_NUMBER, FieldNames::LOAN_AMOUNT, 0.2);
        CHECK(strict.run(makeDoc({{FieldNames::ID_NUMBER, UNKNOWN_ID}}, {{FieldNames::LOAN_AMOUNT, 1000}})).passed);
    }

    SUBCASE("Positive: the same class checks refunds by changing the amount field") {
        FraudCheckAdapter refund(engine, FieldNames::ID_NUMBER, FieldNames::REFUND_AMOUNT, 0.5);
        CHECK(refund.run(makeDoc({{FieldNames::ID_NUMBER, ID}}, {{FieldNames::REFUND_AMOUNT, 300}})).passed);
    }

    SUBCASE("Errors: an amount on the form but left blank gives 'Missing field'") {
        Document doc = makeDoc({{FieldNames::ID_NUMBER, ID}});
        doc.addNumberField(FieldNames::LOAN_AMOUNT);  // on the form, never filled in
        CHECK(adapter.run(doc).message == "Missing field: loanAmount");
    }
}

// =====================================================================================
//                                   THE PATTERN ITSELF
// =====================================================================================

TEST_CASE("Every adapter can be used through the ExternalCheck interface") {
    // Arrange: five different adapters behind one interface (what ExternalCheckNode relies on)
    std::vector<std::shared_ptr<ExternalCheck>> checks;
    checks.push_back(std::make_shared<CreditCheckAdapter>(std::make_shared<LegacyCreditBureau>(),
                                                          FieldNames::ID_NUMBER, 650));
    checks.push_back(std::make_shared<CriminalRecordAdapter>(std::make_shared<PoliceRecordsDB>(),
                                                             FieldNames::ID_NUMBER));
    checks.push_back(std::make_shared<IdentityVerificationAdapter>(
        std::make_shared<HomeAffairsService>(), FieldNames::NAME, FieldNames::SURNAME,
        FieldNames::ID_NUMBER, FieldNames::DATE_OF_BIRTH));
    checks.push_back(std::make_shared<QualificationAdapter>(std::make_shared<ExamBoardAPI>(),
                                                            FieldNames::EXAM_CANDIDATE_NUMBER,
                                                            std::map<std::string, int>()));
    checks.push_back(std::make_shared<FraudCheckAdapter>(std::make_shared<FraudScreeningEngine>(),
                                                         FieldNames::ID_NUMBER, FieldNames::LOAN_AMOUNT, 0.5));

    // Act + Assert: the same call works on every one, and each reports its own name
    Document empty;
    for (std::size_t i = 0; i < checks.size(); ++i) {
        CheckResult r = checks[i]->run(empty);
        CHECK_FALSE(r.passed);
        CHECK(r.message.find("Missing field: ") == 0);
        CHECK(r.checkName == checks[i]->getName());
    }
}
