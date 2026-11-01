#include <gtest/gtest.h>

#include "Requester.h"
#include "AccessAttempt.h"

TEST(RequesterTest, DefaultHasEmptyIdAndLeastPrivilegedRole)
{
    Requester r;

    EXPECT_TRUE(r.userId.empty());
    EXPECT_TRUE(r.role == UserRole::Client);
}

TEST(RequesterTest, ConstructorStoresIdAndRole)
{
    Requester r("0102155009087", UserRole::Staff);

    EXPECT_EQ("0102155009087", r.userId);
    EXPECT_TRUE(r.role == UserRole::Staff);
}

TEST(AccessAttemptTest, DefaultLevelIsDenied)
{
    AccessAttempt a;

    EXPECT_TRUE(a.level == AccessLevel::Denied);
    EXPECT_TRUE(a.timestamp.empty());
    EXPECT_TRUE(a.userId.empty());
    EXPECT_TRUE(a.accountNumber.empty());
}

TEST(AccessAttemptTest, ConstructorStoresAllFields)
{
    AccessAttempt a("2026-10-31 10:00:00", "0102155009087", "L-0001", AccessLevel::Full);

    EXPECT_EQ("2026-10-31 10:00:00", a.timestamp);
    EXPECT_EQ("0102155009087", a.userId);
    EXPECT_EQ("L-0001", a.accountNumber);
    EXPECT_TRUE(a.level == AccessLevel::Full);
}