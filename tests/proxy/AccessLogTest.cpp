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