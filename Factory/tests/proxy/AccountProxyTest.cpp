#include <gtest/gtest.h>

#include <map>
#include <memory>
#include <string>

#include "Account.h"
#include "AccountRegistry.h"
#include "AccessLog.h"
#include "AccountProxy.h"
#include "Requester.h"

/**
 * Test double for Account. Account is abstract (getDetails is pure virtual),
 * so the tests need a concrete subclass with fixed, known values.
 */
class StubAccount : public Account
{
    public:
        // counts calls to describe(); static because describe() is const
        static int describeCalls;

        StubAccount(const std::string& number, const std::string& owner)
            : Account(number, "LoanRequest", owner, "2026-10-31 10:00:00")
        {
        }

        std::string describe() const
        {
            ++describeCalls;
            return Account::describe();
        }

        std::map<std::string, std::string> getDetails() const
        {
            std::map<std::string, std::string> details;
            details["principal"] = "100000";
            return details;
        }
};

int StubAccount::describeCalls = 0;

/**
 * Fixture: a registry holding one account (L-0001, owned by OWNER_ID)
 * and a shared access log.
 */
class AccountProxyTest : public ::testing::Test
{
    protected:
        static const std::string OWNER_ID;
        static const std::string ACCOUNT_NUMBER;

        std::shared_ptr<AccountRegistry> registry;
        std::shared_ptr<AccessLog> log;

        // builds a proxy for one requester and returns what display() shows
        std::string view(const Requester& r, const std::string& accountNumber)
        {
            AccountProxy proxy(registry, accountNumber, r, log);
            return proxy.display();
        }

        static bool contains(const std::string& text, const std::string& part)
        {
            return text.find(part) != std::string::npos;
        }

        void SetUp()
        {
            StubAccount::describeCalls = 0;
            registry = std::make_shared<AccountRegistry>();
            log = std::make_shared<AccessLog>();

            // the registry takes ownership of the raw pointer; do not delete it
            registry->add(new StubAccount(ACCOUNT_NUMBER, OWNER_ID));
        }
};

const std::string AccountProxyTest::OWNER_ID = "0102155009087";
const std::string AccountProxyTest::ACCOUNT_NUMBER = "L-0001";

TEST_F(AccountProxyTest, FixtureBuildsAndRegistryFindsStub)
{
    ASSERT_TRUE(registry->find(ACCOUNT_NUMBER) != nullptr);
    EXPECT_EQ(OWNER_ID, registry->find(ACCOUNT_NUMBER)->getOwnerId());
    EXPECT_EQ(0u, log->getAttempts().size());
}

// ---------- Client ----------

TEST_F(AccountProxyTest, OwnerSeesFullDetails)
{
    Requester owner(OWNER_ID, UserRole::Client);

    std::string out = view(owner, ACCOUNT_NUMBER);

    EXPECT_TRUE(contains(out, ACCOUNT_NUMBER));
    EXPECT_TRUE(contains(out, "principal: 100000"));
    EXPECT_FALSE(contains(out, "Access denied"));
}

TEST_F(AccountProxyTest, OwnerDoesNotSeeInternalNotes)
{
    Requester owner(OWNER_ID, UserRole::Client);

    EXPECT_FALSE(contains(view(owner, ACCOUNT_NUMBER), "Internal notes"));
}

TEST_F(AccountProxyTest, OtherClientIsDenied)
{
    Requester other("9999999999999", UserRole::Client);

    EXPECT_EQ("Access denied", view(other, ACCOUNT_NUMBER));
}

TEST_F(AccountProxyTest, EmptyIdClientIsDenied)
{
    Requester anonymous;   // empty id, Client role

    EXPECT_EQ("Access denied", view(anonymous, ACCOUNT_NUMBER));
}

TEST_F(AccountProxyTest, EmptyIdClientIsDeniedEvenWhenAccountOwnerIsEmpty)
{
    // an empty id must never "match" an empty owner id
    registry->add(new StubAccount("E-0003", ""));
    Requester anonymous;

    EXPECT_EQ("Access denied", view(anonymous, "E-0003"));
}

// ---------- Staff ----------

TEST_F(AccountProxyTest, StaffSeesDetailsAndInternalNotes)
{
    Requester staff("S-1", UserRole::Staff);

    std::string out = view(staff, ACCOUNT_NUMBER);

    EXPECT_TRUE(contains(out, ACCOUNT_NUMBER));
    EXPECT_TRUE(contains(out, "principal: 100000"));
    EXPECT_TRUE(contains(out, "Internal notes"));
}

// ---------- Auditor ----------

TEST_F(AccountProxyTest, AuditorSeesSummaryWithMaskedId)
{
    Requester auditor("A-1", UserRole::Auditor);

    std::string out = view(auditor, ACCOUNT_NUMBER);

    EXPECT_TRUE(contains(out, ACCOUNT_NUMBER));
    EXPECT_TRUE(contains(out, "*********9087"));   // 13 digits, last 4 visible
    EXPECT_FALSE(contains(out, OWNER_ID));         // full id never shown
}

TEST_F(AccountProxyTest, AuditorSummaryHidesDetailsAndNotes)
{
    Requester auditor("A-1", UserRole::Auditor);

    std::string out = view(auditor, ACCOUNT_NUMBER);

    EXPECT_FALSE(contains(out, "principal"));
    EXPECT_FALSE(contains(out, "Internal notes"));
    EXPECT_EQ(0, StubAccount::describeCalls);      // summary never uses describe()
}

TEST_F(AccountProxyTest, FiveCharacterIdKeepsOnlyLastFour)
{
    registry->add(new StubAccount("S-0002", "ABCDE"));
    Requester auditor("A-1", UserRole::Auditor);

    std::string out = view(auditor, "S-0002");

    EXPECT_TRUE(contains(out, "Owner ID: *BCDE"));
    EXPECT_FALSE(contains(out, "ABCDE"));
}

TEST_F(AccountProxyTest, FourCharacterIdIsFullyMasked)
{
    registry->add(new StubAccount("S-0002", "AB12"));
    Requester auditor("A-1", UserRole::Auditor);

    std::string out = view(auditor, "S-0002");

    EXPECT_TRUE(contains(out, "Owner ID: ****"));
    EXPECT_FALSE(contains(out, "AB12"));
}

// ---------- Unknown account ----------

TEST_F(AccountProxyTest, UnknownAccountIsDeniedForClient)
{
    Requester owner(OWNER_ID, UserRole::Client);

    EXPECT_EQ("Access denied", view(owner, "X-9999"));
}

TEST_F(AccountProxyTest, UnknownAccountIsDeniedEvenForStaff)
{
    Requester staff("S-1", UserRole::Staff);

    EXPECT_EQ("Access denied", view(staff, "X-9999"));
}

// ---------- Logging ----------

TEST_F(AccountProxyTest, DisplayLogsExactlyOneAttemptWithTheLevel)
{
    Requester owner(OWNER_ID, UserRole::Client);

    view(owner, ACCOUNT_NUMBER);

    ASSERT_EQ(1u, log->getAttempts().size());
    const AccessAttempt& a = log->getAttempts()[0];
    EXPECT_EQ(OWNER_ID, a.userId);
    EXPECT_EQ(ACCOUNT_NUMBER, a.accountNumber);
    EXPECT_TRUE(a.level == AccessLevel::Full);
    EXPECT_FALSE(a.timestamp.empty());
}

TEST_F(AccountProxyTest, DeniedAttemptIsLoggedAndAppearsInGetDenied)
{
    Requester other("9999999999999", UserRole::Client);

    view(other, ACCOUNT_NUMBER);

    ASSERT_EQ(1u, log->getDenied().size());
    EXPECT_EQ("9999999999999", log->getDenied()[0].userId);
    EXPECT_EQ(ACCOUNT_NUMBER, log->getDenied()[0].accountNumber);
}

TEST_F(AccountProxyTest, SharedLogCollectsAttemptsFromTwoProxies)
{
    Requester owner(OWNER_ID, UserRole::Client);
    Requester staff("S-1", UserRole::Staff);

    view(owner, ACCOUNT_NUMBER);
    view(staff, ACCOUNT_NUMBER);

    ASSERT_EQ(2u, log->getAttempts().size());
    EXPECT_TRUE(log->getAttempts()[0].level == AccessLevel::Full);
    EXPECT_TRUE(log->getAttempts()[1].level == AccessLevel::FullWithNotes);
}

// ---------- Protection: denied never touches the account ----------

TEST_F(AccountProxyTest, DeniedRequestNeverTouchesTheAccount)
{
    Requester other("9999999999999", UserRole::Client);

    view(other, ACCOUNT_NUMBER);

    EXPECT_EQ(0, StubAccount::describeCalls);
}

TEST_F(AccountProxyTest, UnknownAccountRequestNeverTouchesTheAccount)
{
    Requester staff("S-1", UserRole::Staff);

    view(staff, "X-9999");

    EXPECT_EQ(0, StubAccount::describeCalls);
}

// ---------- Subject interface ----------

TEST_F(AccountProxyTest, GetAccountNumberReturnsTheRequestedNumber)
{
    Requester owner(OWNER_ID, UserRole::Client);
    AccountProxy proxy(registry, ACCOUNT_NUMBER, owner, log);

    EXPECT_EQ(ACCOUNT_NUMBER, proxy.getAccountNumber());
}