
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <map>

#include "FinanceAccountFactory.h"
#include "LoanAccount.h"
#include "RefundRecord.h"
#include "DocumentBuilder/Document.h"
#include "WorkflowInstance/WorkflowInstance.h"

// ------------------------------------------------------------
// Test document creation helpers
// ------------------------------------------------------------

std::shared_ptr<Document> createLoanDocument()
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "123456789");
    document->setNumber("principal", 50000.0);
    document->setNumber("interestRate", 10.5);
    document->setNumber("termMonths", 24.0);
    document->setNumber("monthlyInstalment", 2500.5);
    document->setNumber("outstanding", 40000.0);

    return document;
}

std::shared_ptr<Document> createRefundDocument()
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "987654321");
    document->setField("orderNumber", "ORD001");
    document->setNumber("amount", 1250.75);
    document->setField("payoutMethod", "Bank Transfer");

    return document;
}

// ------------------------------------------------------------
// LOAN ACCOUNT TESTS
// ------------------------------------------------------------

TEST(FinanceAccountFactoryTest, CreatesLoanAccount)
{
    std::shared_ptr<Document> document = createLoanDocument();
    WorkflowInstance workflow(document, "LoanApplication");

    FinanceAccountFactory factory("LOAN", "ownerId");

    std::unique_ptr<Account> account = factory.create(workflow);

    ASSERT_NE(account.get(), nullptr);

    EXPECT_NE(dynamic_cast<LoanAccount*>(account.get()), nullptr);

    EXPECT_EQ(account->getWorkflowType(), "LoanApplication");
    EXPECT_EQ(account->getOwnerId(), "123456789");
    EXPECT_EQ(account->getAccountNumber(), "LOAN1");

    std::map<std::string, std::string> details = account->getDetails();

    EXPECT_EQ(details["Principal"], "50000");
    EXPECT_EQ(details["Interest Rate"], "10.5");
    EXPECT_EQ(details["Term (Months)"], "24");
    EXPECT_EQ(details["Monthly Instalment"], "2500.5");
    EXPECT_EQ(details["Outstanding Balance"], "40000");
}

TEST(FinanceAccountFactoryTest, ReturnsNullWhenLoanFieldsAreMissing)
{
    const std::string requiredFields[] = {
        "principal",
        "interestRate",
        "termMonths",
        "monthlyInstalment",
        "outstanding"
    };

    for (int i = 0; i < 5; ++i)
    {
        SCOPED_TRACE(requiredFields[i]);

        std::shared_ptr<Document> document(new Document());

        document->setField("ownerId", "123456789");

        if (requiredFields[i] != "principal")
            document->setNumber("principal", 50000.0);

        if (requiredFields[i] != "interestRate")
            document->setNumber("interestRate", 10.5);

        if (requiredFields[i] != "termMonths")
            document->setNumber("termMonths", 24.0);

        if (requiredFields[i] != "monthlyInstalment")
            document->setNumber("monthlyInstalment", 2500.5);

        if (requiredFields[i] != "outstanding")
            document->setNumber("outstanding", 40000.0);

        WorkflowInstance workflow(document, "LoanApplication");
        FinanceAccountFactory factory("LOAN", "ownerId");

        EXPECT_EQ(factory.create(workflow).get(), nullptr);
    }
}

// ------------------------------------------------------------
// REFUND ACCOUNT TESTS
// ------------------------------------------------------------

TEST(FinanceAccountFactoryTest, CreatesRefundRecord)
{
    std::shared_ptr<Document> document = createRefundDocument();
    WorkflowInstance workflow(document, "Refund");

    FinanceAccountFactory factory("REF", "ownerId");

    std::unique_ptr<Account> account = factory.create(workflow);

    ASSERT_NE(account.get(), nullptr);

    EXPECT_NE(dynamic_cast<RefundRecord*>(account.get()), nullptr);

    EXPECT_EQ(account->getWorkflowType(), "Refund");
    EXPECT_EQ(account->getOwnerId(), "987654321");
    EXPECT_EQ(account->getAccountNumber(), "REF1");

    std::map<std::string, std::string> details = account->getDetails();

    EXPECT_EQ(details["Order Number"], "ORD001");
    EXPECT_EQ(details["Refund Amount"], "1250.75");
    EXPECT_EQ(details["Payout Method"], "Bank Transfer");
}

TEST(FinanceAccountFactoryTest, ReturnsNullWhenRefundFieldsAreMissing)
{
    const std::string requiredFields[] = {
        "orderNumber",
        "amount",
        "payoutMethod"
    };

    for (int i = 0; i < 3; ++i)
    {
        SCOPED_TRACE(requiredFields[i]);

        std::shared_ptr<Document> document(new Document());

        document->setField("ownerId", "987654321");

        if (requiredFields[i] != "orderNumber")
            document->setField("orderNumber", "ORD001");

        if (requiredFields[i] != "amount")
            document->setNumber("amount", 1250.75);

        if (requiredFields[i] != "payoutMethod")
            document->setField("payoutMethod", "Bank Transfer");

        WorkflowInstance workflow(document, "Refund");
        FinanceAccountFactory factory("REF", "ownerId");

        EXPECT_EQ(factory.create(workflow).get(), nullptr);
    }
}

// ------------------------------------------------------------
// GENERAL FINANCE FACTORY VALIDATION TESTS
// ------------------------------------------------------------

TEST(FinanceAccountFactoryTest, ReturnsNullForNullDocument)
{
    std::shared_ptr<Document> document;

    WorkflowInstance workflow(document, "LoanApplication");
    FinanceAccountFactory factory("LOAN", "ownerId");

    EXPECT_EQ(factory.create(workflow).get(), nullptr);
}

TEST(FinanceAccountFactoryTest, ReturnsNullWhenOwnerIdIsMissing)
{
    std::shared_ptr<Document> document = createLoanDocument();

    // Create a document without the owner ID.
    document.reset(new Document());

    document->setNumber("principal", 50000.0);
    document->setNumber("interestRate", 10.5);
    document->setNumber("termMonths", 24.0);
    document->setNumber("monthlyInstalment", 2500.5);
    document->setNumber("outstanding", 40000.0);

    WorkflowInstance workflow(document, "LoanApplication");
    FinanceAccountFactory factory("LOAN", "ownerId");

    EXPECT_EQ(factory.create(workflow).get(), nullptr);
}

TEST(FinanceAccountFactoryTest, ReturnsNullForUnsupportedWorkflowType)
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "123456789");

    WorkflowInstance workflow(document, "UnknownWorkflow");
    FinanceAccountFactory factory("FIN", "ownerId");

    EXPECT_EQ(factory.create(workflow).get(), nullptr);
}
