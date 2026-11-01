#include <gtest/gtest.h>

#include "AccessLog.h"
#include "Requester.h"

TEST(AccessLogTest, NewLogIsEmpty)
{
    AccessLog log;

    EXPECT_EQ(0u, log.getAttempts().size());
}

TEST(AccessLogTest, RecordStoresAttemptCorrectly)
{
    AccessLog log;
    Requester requester("0102155009087", UserRole::Client);

    log.record(requester, "L-0001", AccessLevel::Full);

    // ASSERT, not EXPECT: the lines below index into the vector
    ASSERT_EQ(1u, log.getAttempts().size());

    const AccessAttempt& attempt = log.getAttempts()[0];
    EXPECT_EQ("0102155009087", attempt.userId);
    EXPECT_EQ("L-0001", attempt.accountNumber);
    EXPECT_TRUE(attempt.level == AccessLevel::Full);
    EXPECT_FALSE(attempt.timestamp.empty());
}

TEST(AccessLogTest, GetDeniedOnEmptyLogIsEmpty)
{
    AccessLog log;

    EXPECT_EQ(0u, log.getDenied().size());
}

TEST(AccessLogTest, GetDeniedReturnsOnlyDeniedAttempts)
{
    AccessLog log;
    Requester requester("0102155009087", UserRole::Client);

    log.record(requester, "A-1", AccessLevel::Denied);
    log.record(requester, "B-2", AccessLevel::Full);
    log.record(requester, "C-3", AccessLevel::Denied);

    std::vector<AccessAttempt> denied = log.getDenied();

    ASSERT_EQ(2u, denied.size());
    EXPECT_EQ("A-1", denied[0].accountNumber);
    EXPECT_EQ("C-3", denied[1].accountNumber);
    EXPECT_TRUE(denied[0].level == AccessLevel::Denied);
    EXPECT_TRUE(denied[1].level == AccessLevel::Denied);
}

TEST(AccessLogTest, GetDeniedWithNoDeniedAttemptsIsEmpty)
{
    AccessLog log;
    Requester requester("0102155009087", UserRole::Staff);

    log.record(requester, "A-1", AccessLevel::FullWithNotes);
    log.record(requester, "B-2", AccessLevel::Summary);

    EXPECT_EQ(0u, log.getDenied().size());
}

TEST(AccessLogTest, GetDeniedLeavesLogUnchanged)
{
    AccessLog log;
    Requester requester("0102155009087", UserRole::Client);

    log.record(requester, "A-1", AccessLevel::Denied);
    log.record(requester, "B-2", AccessLevel::Full);
    log.record(requester, "C-3", AccessLevel::Denied);

    log.getDenied();

    EXPECT_EQ(3u, log.getAttempts().size());
}

TEST(AccessLogTest, AttemptsKeepTheOrderTheyWereRecorded)
{
    AccessLog log;
    Requester requester("0102155009087", UserRole::Auditor);

    log.record(requester, "A-1", AccessLevel::Summary);
    log.record(requester, "B-2", AccessLevel::Denied);

    ASSERT_EQ(2u, log.getAttempts().size());
    EXPECT_EQ("A-1", log.getAttempts()[0].accountNumber);
    EXPECT_EQ("B-2", log.getAttempts()[1].accountNumber);
}