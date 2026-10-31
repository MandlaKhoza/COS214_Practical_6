#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <map>
#include <vector>

#include "GeneralAccountFactory.h"
#include "StudentAccount.h"
#include "EmployeeAccount.h"
#include "SurveyReceipt.h"
#include "DocumentBuilder/Document.h"
#include "WorkflowInstance/WorkflowInstance.h"

// Checks whether a detail value exists without depending on the map's keys.
bool detailsContainValue(const Account& account, const std::string& expected)
{
    std::map<std::string, std::string> details = account.getDetails();

    for (std::map<std::string, std::string>::const_iterator it = details.begin();
         it != details.end(); ++it)
    {
        if (it->second == expected)
        {
            return true;
        }
    }

    return false;
}

// Creates a document containing all required student fields.
std::shared_ptr<Document> createStudentDocument()
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "123456789");
    document->setField("studentNumber", "u12345678");
    document->setField("faculty", "Engineering");
    document->setField("programme", "Computer Science");

    return document;
}

// Creates a document containing all required employee fields.
std::shared_ptr<Document> createEmployeeDocument()
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "987654321");
    document->setField("employeeNumber", "EMP001");
    document->setField("department", "Computer Science");
    document->setField("systemAccess", "Administrator");

    return document;
}

// Creates a document containing all required survey fields.
std::shared_ptr<Document> createSurveyDocument()
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "SURVEY001");
    document->setField("responseId", "RESP001");
    document->setNumber("summaryScore", 85.5);

    return document;
}


// ============================================================
// STUDENT ACCOUNT TESTS
// ============================================================

TEST(GeneralAccountFactoryTest, CreatesStudentAccount)
{
    std::shared_ptr<Document> document = createStudentDocument();
    WorkflowInstance workflow(document, "StudentApplication");

    GeneralAccountFactory factory("STU", "ownerId");

    std::unique_ptr<Account> account = factory.create(workflow);

    ASSERT_NE(account.get(), nullptr);
    EXPECT_NE(dynamic_cast<StudentAccount*>(account.get()), nullptr);

    EXPECT_EQ(account->getWorkflowType(), "StudentApplication");
    EXPECT_EQ(account->getOwnerId(), "123456789");

    EXPECT_TRUE(detailsContainValue(*account, "u12345678"));
    EXPECT_TRUE(detailsContainValue(*account, "Engineering"));
    EXPECT_TRUE(detailsContainValue(*account, "Computer Science"));
}

TEST(GeneralAccountFactoryTest, ReturnsNullForStudentMissingRequiredFields)
{
    const std::string requiredFields[] = {
        "studentNumber",
        "faculty",
        "programme"
    };

    for (int i = 0; i < 3; ++i)
    {
        SCOPED_TRACE(requiredFields[i]);

        std::shared_ptr<Document> document(new Document());

        document->setField("ownerId", "123456789");

        if (requiredFields[i] != "studentNumber")
        {
            document->setField("studentNumber", "u12345678");
        }

        if (requiredFields[i] != "faculty")
        {
            document->setField("faculty", "Engineering");
        }

        if (requiredFields[i] != "programme")
        {
            document->setField("programme", "Computer Science");
        }

        WorkflowInstance workflow(document, "StudentApplication");
        GeneralAccountFactory factory("STU", "ownerId");

        EXPECT_EQ(factory.create(workflow).get(), nullptr);
    }
}


// ============================================================
// EMPLOYEE ACCOUNT TESTS
// ============================================================

TEST(GeneralAccountFactoryTest, CreatesEmployeeAccount)
{
    std::shared_ptr<Document> document = createEmployeeDocument();
    WorkflowInstance workflow(document, "EmployeeApplication");

    GeneralAccountFactory factory("EMP", "ownerId");

    std::unique_ptr<Account> account = factory.create(workflow);

    ASSERT_NE(account.get(), nullptr);
    EXPECT_NE(dynamic_cast<EmployeeAccount*>(account.get()), nullptr);

    EXPECT_EQ(account->getWorkflowType(), "EmployeeApplication");
    EXPECT_EQ(account->getOwnerId(), "987654321");

    EXPECT_TRUE(detailsContainValue(*account, "EMP001"));
    EXPECT_TRUE(detailsContainValue(*account, "Computer Science"));
    EXPECT_TRUE(detailsContainValue(*account, "Administrator"));
}

TEST(GeneralAccountFactoryTest, ReturnsNullForEmployeeMissingRequiredFields)
{
    const std::string requiredFields[] = {
        "employeeNumber",
        "department",
        "systemAccess"
    };

    for (int i = 0; i < 3; ++i)
    {
        SCOPED_TRACE(requiredFields[i]);

        std::shared_ptr<Document> document(new Document());

        document->setField("ownerId", "987654321");

        if (requiredFields[i] != "employeeNumber")
        {
            document->setField("employeeNumber", "EMP001");
        }

        if (requiredFields[i] != "department")
        {
            document->setField("department", "Computer Science");
        }

        if (requiredFields[i] != "systemAccess")
        {
            document->setField("systemAccess", "Administrator");
        }

        WorkflowInstance workflow(document, "EmployeeApplication");
        GeneralAccountFactory factory("EMP", "ownerId");

        EXPECT_EQ(factory.create(workflow).get(), nullptr);
    }
}


// ============================================================
// SURVEY RECEIPT TESTS
// ============================================================

TEST(GeneralAccountFactoryTest, CreatesSurveyReceipt)
{
    std::shared_ptr<Document> document = createSurveyDocument();
    WorkflowInstance workflow(document, "Survey");

    GeneralAccountFactory factory("SUR", "ownerId");

    std::unique_ptr<Account> account = factory.create(workflow);

    ASSERT_NE(account.get(), nullptr);
    EXPECT_NE(dynamic_cast<SurveyReceipt*>(account.get()), nullptr);

    EXPECT_EQ(account->getWorkflowType(), "Survey");
    EXPECT_EQ(account->getOwnerId(), "SURVEY001");

    EXPECT_TRUE(detailsContainValue(*account, "RESP001"));
}

TEST(GeneralAccountFactoryTest, ReturnsNullForSurveyMissingResponseId)
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "SURVEY001");
    document->setNumber("summaryScore", 85.5);

    WorkflowInstance workflow(document, "Survey");
    GeneralAccountFactory factory("SUR", "ownerId");

    EXPECT_EQ(factory.create(workflow).get(), nullptr);
}

TEST(GeneralAccountFactoryTest, ReturnsNullForSurveyMissingSummaryScore)
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "SURVEY001");
    document->setField("responseId", "RESP001");

    WorkflowInstance workflow(document, "Survey");
    GeneralAccountFactory factory("SUR", "ownerId");

    EXPECT_EQ(factory.create(workflow).get(), nullptr);
}


// ============================================================
// GENERAL FACTORY VALIDATION TESTS
// ============================================================

TEST(GeneralAccountFactoryTest, ReturnsNullForNullDocument)
{
    std::shared_ptr<Document> document;
    WorkflowInstance workflow(document, "StudentApplication");

    GeneralAccountFactory factory("STU", "ownerId");

    EXPECT_EQ(factory.create(workflow).get(), nullptr);
}

TEST(GeneralAccountFactoryTest, ReturnsNullWhenOwnerIdIsMissing)
{
    std::shared_ptr<Document> document(new Document());

    document->setField("studentNumber", "u12345678");
    document->setField("faculty", "Engineering");
    document->setField("programme", "Computer Science");

    WorkflowInstance workflow(document, "StudentApplication");
    GeneralAccountFactory factory("STU", "ownerId");

    EXPECT_EQ(factory.create(workflow).get(), nullptr);
}

TEST(GeneralAccountFactoryTest, ReturnsNullForUnsupportedWorkflowType)
{
    std::shared_ptr<Document> document(new Document());

    document->setField("ownerId", "123456789");

    WorkflowInstance workflow(document, "UnknownWorkflow");
    GeneralAccountFactory factory("ACC", "ownerId");

    EXPECT_EQ(factory.create(workflow).get(), nullptr);
}