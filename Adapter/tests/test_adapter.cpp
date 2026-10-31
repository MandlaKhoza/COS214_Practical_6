/**
 * @file test_adapter.cpp
 * @brief GoogleTest unit tests for the Adapter pattern: the five adaptees
 *        (external-system stubs) and the five adapters.
 *
 * Each class has a test fixture (its shared Arrange step in SetUp()), and each
 * scenario is one TEST_F written as Arrange / Act / Assert. Test names start with
 * the kind of case they cover: Positive, Negative, Edge or Errors. The Document
 * dependency is mocked by tests/support/DocumentStub.cpp.
 *
 * @author Hafani Ramagoma
 * @date 2026-10-31
 */
#include <gtest/gtest.h>

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

const std::string ID = "0102155009087";         ///< a test applicant's ID number
const std::string OTHER_ID = "9001015800085";   ///< a second test applicant
const std::string UNKNOWN_ID = "0000000000000"; ///< an ID no external system knows

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

/// @brief Fixture: a credit bureau that knows one person (score 712).
class LegacyCreditBureauTest : public ::testing::Test {
protected:
    void SetUp() override { bureau.addRecord(CreditRecord{ID, 712, 15000.0, false}); }
    LegacyCreditBureau bureau;
};

TEST_F(LegacyCreditBureauTest, Positive_ReturnsScoreOfKnownPerson) {
    // Act
    int score = bureau.fetchScore(ID);
    // Assert
    EXPECT_EQ(score, 712);
}

TEST_F(LegacyCreditBureauTest, Positive_FindRecordReturnsFullRecord) {
    const CreditRecord* record = bureau.findRecord(ID);
    ASSERT_NE(record, nullptr);  // stop here if missing, so we never dereference nullptr
    EXPECT_DOUBLE_EQ(record->outstandingDebt, 15000.0);
    EXPECT_FALSE(record->hasDefault);
}

TEST_F(LegacyCreditBureauTest, Negative_UnknownPersonGivesMinusOne) {
    EXPECT_EQ(bureau.fetchScore(UNKNOWN_ID), -1);
}

TEST_F(LegacyCreditBureauTest, Negative_FindRecordReturnsNullForUnknownId) {
    EXPECT_EQ(bureau.findRecord(UNKNOWN_ID), nullptr);
}

TEST_F(LegacyCreditBureauTest, Edge_AddingSameIdTwiceKeepsNewerRecord) {
    bureau.addRecord(CreditRecord{ID, 540, 90000.0, true});
    EXPECT_EQ(bureau.fetchScore(ID), 540);
}

TEST_F(LegacyCreditBureauTest, Edge_ScoreOfZeroIsARealScore) {
    bureau.addRecord(CreditRecord{OTHER_ID, 0, 0.0, false});
    EXPECT_EQ(bureau.fetchScore(OTHER_ID), 0);
}

TEST_F(LegacyCreditBureauTest, Errors_LookingUpUnknownPersonNeverThrows) {
    EXPECT_NO_THROW(bureau.fetchScore(UNKNOWN_ID));
}

/// @brief Fixture: a police database with one person who has a record.
class PoliceRecordsDBTest : public ::testing::Test {
protected:
    void SetUp() override { db.addRecord(CriminalRecord{ID, true, "Fraud (2019)"}); }
    PoliceRecordsDB db;
};

TEST_F(PoliceRecordsDBTest, Positive_LookupReturnsStoredRecord) {
    const CriminalRecord* record = db.lookup(ID);
    ASSERT_NE(record, nullptr);
    EXPECT_TRUE(record->hasRecord);
    EXPECT_EQ(record->offenceSummary, "Fraud (2019)");
}

TEST_F(PoliceRecordsDBTest, Negative_LookupReturnsNullForUnknownId) {
    EXPECT_EQ(db.lookup(UNKNOWN_ID), nullptr);
}

TEST_F(PoliceRecordsDBTest, Edge_AddingSameIdTwiceKeepsNewerRecord) {
    db.addRecord(CriminalRecord{ID, false, ""});
    ASSERT_NE(db.lookup(ID), nullptr);
    EXPECT_FALSE(db.lookup(ID)->hasRecord);
}

TEST_F(PoliceRecordsDBTest, Errors_LookingUpUnknownPersonNeverThrows) {
    EXPECT_NO_THROW(db.lookup(UNKNOWN_ID));
}

/// @brief Fixture: a Home Affairs register with one person.
class HomeAffairsServiceTest : public ::testing::Test {
protected:
    void SetUp() override { service.addRecord(IdentityRecord{ID, "Hafs", "Ramagoma", "2001-02-15", true}); }
    HomeAffairsService service;
};

TEST_F(HomeAffairsServiceTest, Positive_MatchingDetailsAreVerified) {
    EXPECT_TRUE(service.verifyIdentity("Hafs", "Ramagoma", ID, "2001-02-15"));
}

TEST_F(HomeAffairsServiceTest, Negative_DifferentSurnameIsNotVerified) {
    EXPECT_FALSE(service.verifyIdentity("Hafs", "Smith", ID, "2001-02-15"));
}

TEST_F(HomeAffairsServiceTest, Negative_DifferentDateOfBirthIsNotVerified) {
    EXPECT_FALSE(service.verifyIdentity("Hafs", "Ramagoma", ID, "2001-02-16"));
}

TEST_F(HomeAffairsServiceTest, Negative_UnknownIdIsNotVerified) {
    EXPECT_FALSE(service.verifyIdentity("Hafs", "Ramagoma", UNKNOWN_ID, "2001-02-15"));
}

TEST_F(HomeAffairsServiceTest, Edge_ComparisonIsCaseSensitive) {
    EXPECT_FALSE(service.verifyIdentity("hafs", "Ramagoma", ID, "2001-02-15"));
}

TEST_F(HomeAffairsServiceTest, Positive_FindRecordReturnsStoredRecord) {
    const IdentityRecord* record = service.findRecord(ID);
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->surname, "Ramagoma");
}

/// @brief Fixture: an exam board with one candidate.
class ExamBoardAPITest : public ::testing::Test {
protected:
    void SetUp() override {
        api.addRecord(ExamRecord{"C1001", "Hafs Ramagoma", {{"Mathematics", 78}, {"English", 65}}});
    }
    ExamBoardAPI api;
};

TEST_F(ExamBoardAPITest, Positive_ReturnsCandidateMarks) {
    std::map<std::string, int> results = api.getResults("C1001");
    EXPECT_EQ(results.size(), 2u);
    EXPECT_EQ(results["Mathematics"], 78);
}

TEST_F(ExamBoardAPITest, Negative_UnknownCandidateGivesNoResults) {
    EXPECT_TRUE(api.getResults("C9999").empty());
    EXPECT_EQ(api.findRecord("C9999"), nullptr);
}

TEST_F(ExamBoardAPITest, Edge_AddingSameCandidateTwiceKeepsNewerRecord) {
    api.addRecord(ExamRecord{"C1001", "Hafs Ramagoma", {{"Mathematics", 40}}});
    std::map<std::string, int> results = api.getResults("C1001");
    EXPECT_EQ(results.size(), 1u);
    EXPECT_EQ(results["Mathematics"], 40);
}

/// @brief Fixture: a fraud engine with a low-risk person and a flagged person.
class FraudScreeningEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine.addProfile(FraudProfile{ID, 0.1, false, ""});
        engine.addProfile(FraudProfile{OTHER_ID, 0.3, true, "Identity theft report"});
    }
    FraudScreeningEngine engine;
};

TEST_F(FraudScreeningEngineTest, Positive_KnownPersonBaseRiskForSmallAmounts) {
    EXPECT_DOUBLE_EQ(engine.riskScore(ID, 50000), 0.1);
}

TEST_F(FraudScreeningEngineTest, Positive_UnknownPersonStartsAtPointTwo) {
    EXPECT_DOUBLE_EQ(engine.riskScore(UNKNOWN_ID, 50000), 0.2);
}

TEST_F(FraudScreeningEngineTest, Negative_FlaggedPersonIsAlwaysOne) {
    EXPECT_DOUBLE_EQ(engine.riskScore(OTHER_ID, 10), 1.0);
}

TEST_F(FraudScreeningEngineTest, Edge_ExactlyR100000AddsNothingJustAboveAddsPointOne) {
    EXPECT_NEAR(engine.riskScore(ID, 100000.0), 0.1, 1e-9);
    EXPECT_NEAR(engine.riskScore(ID, 100000.01), 0.2, 1e-9);
}

TEST_F(FraudScreeningEngineTest, Edge_AboveR500000AddsPointTwo) {
    EXPECT_NEAR(engine.riskScore(ID, 900000.0), 0.3, 1e-9);
}

TEST_F(FraudScreeningEngineTest, Edge_RiskIsCappedAtOne) {
    engine.addProfile(FraudProfile{ID, 0.95, false, ""});
    EXPECT_DOUBLE_EQ(engine.riskScore(ID, 900000.0), 1.0);
}

// =====================================================================================
//                                        ADAPTERS
// =====================================================================================

/// @brief Fixture: the mock bureau (scores 712 and 590) and an adapter with minimum 650.
class CreditCheckAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        bureau = std::make_shared<LegacyCreditBureau>();
        bureau->addRecord(CreditRecord{ID, 712, 15000.0, false});
        bureau->addRecord(CreditRecord{OTHER_ID, 590, 80000.0, true});
        adapter.reset(new CreditCheckAdapter(bureau, FieldNames::ID_NUMBER, 650));
    }
    std::shared_ptr<LegacyCreditBureau> bureau;
    std::unique_ptr<CreditCheckAdapter> adapter;
};

TEST_F(CreditCheckAdapterTest, Positive_PassesWhenScoreMeetsMinimum) {
    // Act
    CheckResult r = adapter->run(makeDoc({{FieldNames::ID_NUMBER, ID}}));
    // Assert
    EXPECT_TRUE(r.passed);
    EXPECT_EQ(r.checkName, "Credit check");
    EXPECT_EQ(r.message, "Credit score 712 meets minimum 650");
    EXPECT_EQ(r.data["score"], "712");
}

TEST_F(CreditCheckAdapterTest, Negative_RejectsScoreBelowMinimum) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::ID_NUMBER, OTHER_ID}}));
    EXPECT_FALSE(r.passed);
    EXPECT_EQ(r.message, "Credit score 590 below minimum 650");
}

TEST_F(CreditCheckAdapterTest, Negative_FailsWhenBureauDoesNotKnowPerson) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::ID_NUMBER, UNKNOWN_ID}}));
    EXPECT_FALSE(r.passed);
    EXPECT_EQ(r.message, "No credit record found for ID " + UNKNOWN_ID);
}

TEST_F(CreditCheckAdapterTest, Edge_ScoreExactlyAtMinimumPasses) {
    CreditCheckAdapter strict(bureau, FieldNames::ID_NUMBER, 712);
    EXPECT_TRUE(strict.run(makeDoc({{FieldNames::ID_NUMBER, ID}})).passed);
}

TEST_F(CreditCheckAdapterTest, Errors_IdOnFormButBlankGivesMissingField) {
    Document doc;
    doc.addTextField(FieldNames::ID_NUMBER);  // on the form, never filled in
    CheckResult r = adapter->run(doc);
    EXPECT_FALSE(r.passed);
    EXPECT_EQ(r.message, "Missing field: idNumber");
}

TEST_F(CreditCheckAdapterTest, Errors_EmptyDocumentGivesMissingFieldAndDoesNotThrow) {
    EXPECT_NO_THROW(adapter->run(Document()));
    EXPECT_EQ(adapter->run(Document()).message, "Missing field: idNumber");
}

/// @brief Fixture: a police database (one record, one clean entry) and the adapter.
class CriminalRecordAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        db = std::make_shared<PoliceRecordsDB>();
        db->addRecord(CriminalRecord{ID, true, "Fraud (2019)"});
        db->addRecord(CriminalRecord{OTHER_ID, false, ""});
        adapter.reset(new CriminalRecordAdapter(db, FieldNames::ID_NUMBER));
    }
    std::shared_ptr<PoliceRecordsDB> db;
    std::unique_ptr<CriminalRecordAdapter> adapter;
};

TEST_F(CriminalRecordAdapterTest, Positive_PersonWithNoEntryIsClean) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::ID_NUMBER, UNKNOWN_ID}}));
    EXPECT_TRUE(r.passed);
    EXPECT_EQ(r.message, "No criminal record");
    EXPECT_EQ(r.data["hasRecord"], "false");
}

TEST_F(CriminalRecordAdapterTest, Positive_EntrySayingNoRecordIsClean) {
    EXPECT_TRUE(adapter->run(makeDoc({{FieldNames::ID_NUMBER, OTHER_ID}})).passed);
}

TEST_F(CriminalRecordAdapterTest, Negative_RejectsPersonWithRecordNamingOffence) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::ID_NUMBER, ID}}));
    EXPECT_FALSE(r.passed);
    EXPECT_EQ(r.message, "Criminal record found: Fraud (2019)");
    EXPECT_EQ(r.data["hasRecord"], "true");
}

TEST_F(CriminalRecordAdapterTest, Errors_MissingIdGivesMissingField) {
    EXPECT_EQ(adapter->run(Document()).message, "Missing field: idNumber");
}

/// @brief Fixture: Home Affairs with one person and the identity adapter.
class IdentityVerificationAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        service = std::make_shared<HomeAffairsService>();
        service->addRecord(IdentityRecord{ID, "Hafs", "Ramagoma", "2001-02-15", true});
        adapter.reset(new IdentityVerificationAdapter(service, FieldNames::NAME, FieldNames::SURNAME,
                                                      FieldNames::ID_NUMBER, FieldNames::DATE_OF_BIRTH));
    }
    std::shared_ptr<HomeAffairsService> service;
    std::unique_ptr<IdentityVerificationAdapter> adapter;
};

TEST_F(IdentityVerificationAdapterTest, Positive_PassesWhenAllFourDetailsMatch) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::NAME, "Hafs"}, {FieldNames::SURNAME, "Ramagoma"},
                                          {FieldNames::ID_NUMBER, ID}, {FieldNames::DATE_OF_BIRTH, "2001-02-15"}}));
    EXPECT_TRUE(r.passed);
    EXPECT_EQ(r.message, "Identity confirmed by Home Affairs");
}

TEST_F(IdentityVerificationAdapterTest, Negative_RejectsWhenOneDetailDiffers) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::NAME, "Hafs"}, {FieldNames::SURNAME, "Smith"},
                                          {FieldNames::ID_NUMBER, ID}, {FieldNames::DATE_OF_BIRTH, "2001-02-15"}}));
    EXPECT_FALSE(r.passed);
    EXPECT_EQ(r.message, "Identity details do not match Home Affairs records");
}

TEST_F(IdentityVerificationAdapterTest, Errors_NamesFirstMissingField) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::NAME, "Hafs"}, {FieldNames::ID_NUMBER, ID},
                                          {FieldNames::DATE_OF_BIRTH, "2001-02-15"}}));
    EXPECT_EQ(r.message, "Missing field: surname");
}

TEST_F(IdentityVerificationAdapterTest, Edge_WithEveryFieldMissingNameIsReportedFirst) {
    EXPECT_EQ(adapter->run(Document()).message, "Missing field: name");
}

/// @brief Fixture: an exam board (one strong, one weak candidate) and an adapter requiring 50 in two subjects.
class QualificationAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        api = std::make_shared<ExamBoardAPI>();
        api->addRecord(ExamRecord{"C1001", "Hafs Ramagoma", {{"Mathematics", 78}, {"English", 65}}});
        api->addRecord(ExamRecord{"C1002", "Thabo Mokoena", {{"Mathematics", 45}, {"English", 70}}});
        required["Mathematics"] = 50;
        required["English"] = 50;
        adapter.reset(new QualificationAdapter(api, FieldNames::EXAM_CANDIDATE_NUMBER, required));
    }
    std::shared_ptr<ExamBoardAPI> api;
    std::map<std::string, int> required;
    std::unique_ptr<QualificationAdapter> adapter;
};

TEST_F(QualificationAdapterTest, Positive_PassesWhenEveryRequiredMarkIsMet) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C1001"}}));
    EXPECT_TRUE(r.passed);
    EXPECT_EQ(r.data["Mathematics"], "78");
}

TEST_F(QualificationAdapterTest, Negative_RejectsMarkBelowMinimumNamingIt) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C1002"}}));
    EXPECT_FALSE(r.passed);
    EXPECT_EQ(r.message, "Required marks not met: Mathematics 45 < 50");
}

TEST_F(QualificationAdapterTest, Negative_RejectsRequiredSubjectNotTaken) {
    std::map<std::string, int> withScience = required;
    withScience["Physical Sciences"] = 50;
    QualificationAdapter science(api, FieldNames::EXAM_CANDIDATE_NUMBER, withScience);
    EXPECT_EQ(science.run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C1001"}})).message,
              "Required marks not met: Physical Sciences not taken");
}

TEST_F(QualificationAdapterTest, Negative_FailsWhenExamBoardHasNoResults) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C9999"}}));
    EXPECT_FALSE(r.passed);
    EXPECT_EQ(r.message, "No exam results found for candidate C9999");
}

TEST_F(QualificationAdapterTest, Edge_MarkExactlyAtMinimumPasses) {
    std::map<std::string, int> exact;
    exact["English"] = 65;
    QualificationAdapter exactAdapter(api, FieldNames::EXAM_CANDIDATE_NUMBER, exact);
    EXPECT_TRUE(exactAdapter.run(makeDoc({{FieldNames::EXAM_CANDIDATE_NUMBER, "C1001"}})).passed);
}

TEST_F(QualificationAdapterTest, Errors_MissingCandidateNumberGivesMissingField) {
    EXPECT_EQ(adapter->run(Document()).message, "Missing field: examCandidateNumber");
}

/// @brief Fixture: a fraud engine (low-risk and flagged person) and an adapter with maximum risk 0.5.
class FraudCheckAdapterTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_shared<FraudScreeningEngine>();
        engine->addProfile(FraudProfile{ID, 0.1, false, ""});
        engine->addProfile(FraudProfile{OTHER_ID, 0.3, true, "Identity theft report"});
        adapter.reset(new FraudCheckAdapter(engine, FieldNames::ID_NUMBER, FieldNames::LOAN_AMOUNT, 0.5));
    }
    std::shared_ptr<FraudScreeningEngine> engine;
    std::unique_ptr<FraudCheckAdapter> adapter;
};

TEST_F(FraudCheckAdapterTest, Positive_PassesLowRiskPersonAndSmallAmount) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::ID_NUMBER, ID}}, {{FieldNames::LOAN_AMOUNT, 50000}}));
    EXPECT_TRUE(r.passed);
    EXPECT_EQ(r.data["risk"], "0.10");
    EXPECT_EQ(r.message, "Fraud risk 0.10 within limit 0.50");
}

TEST_F(FraudCheckAdapterTest, Negative_RejectsFlaggedPersonWhateverTheAmount) {
    CheckResult r = adapter->run(makeDoc({{FieldNames::ID_NUMBER, OTHER_ID}}, {{FieldNames::LOAN_AMOUNT, 1000}}));
    EXPECT_FALSE(r.passed);
    EXPECT_EQ(r.message, "Fraud risk 1.00 above maximum 0.50");
}

TEST_F(FraudCheckAdapterTest, Edge_RiskExactlyAtMaximumPasses) {
    FraudCheckAdapter strict(engine, FieldNames::ID_NUMBER, FieldNames::LOAN_AMOUNT, 0.2);
    EXPECT_TRUE(strict.run(makeDoc({{FieldNames::ID_NUMBER, UNKNOWN_ID}}, {{FieldNames::LOAN_AMOUNT, 1000}})).passed);
}

TEST_F(FraudCheckAdapterTest, Positive_SameClassChecksRefundsByChangingAmountField) {
    FraudCheckAdapter refund(engine, FieldNames::ID_NUMBER, FieldNames::REFUND_AMOUNT, 0.5);
    EXPECT_TRUE(refund.run(makeDoc({{FieldNames::ID_NUMBER, ID}}, {{FieldNames::REFUND_AMOUNT, 300}})).passed);
}

TEST_F(FraudCheckAdapterTest, Errors_AmountOnFormButBlankGivesMissingField) {
    Document doc = makeDoc({{FieldNames::ID_NUMBER, ID}});
    doc.addNumberField(FieldNames::LOAN_AMOUNT);  // on the form, never filled in
    EXPECT_EQ(adapter->run(doc).message, "Missing field: loanAmount");
}

// =====================================================================================
//                                   THE PATTERN ITSELF
// =====================================================================================

TEST(AdapterPatternTest, EveryAdapterCanBeUsedThroughExternalCheckInterface) {
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
        EXPECT_FALSE(r.passed);
        EXPECT_EQ(r.message.find("Missing field: "), 0u);
        EXPECT_EQ(r.checkName, checks[i]->getName());
    }
}
