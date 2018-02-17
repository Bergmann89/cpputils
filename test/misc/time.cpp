#include <chrono>
#include <gtest/gtest.h>
#include <cpputils/misc/time.h>

using namespace ::utl;

TEST(TimeTest, duration_to_timespec)
{
    auto ts1 = duration_cast<timespec>(std::chrono::milliseconds(123456));
    EXPECT_EQ(123,       ts1.tv_sec);
    EXPECT_EQ(456000000, ts1.tv_nsec);
}

TEST(TimeTest, duration_to_timeval)
{
    auto tv1 = duration_cast<timeval>(std::chrono::milliseconds(123456));
    EXPECT_EQ(123,    tv1.tv_sec);
    EXPECT_EQ(456000, tv1.tv_usec);
}

TEST(TimeTest, timespec_to_duration)
{
    auto d1 = duration_cast<std::chrono::microseconds>(timespec{ 1, 234567890 });
    EXPECT_EQ(1234567, d1.count());
}

TEST(TimeTest, timeval_to_duration)
{
    auto d1 = duration_cast<std::chrono::microseconds>(timeval{ 1, 234567 });
    EXPECT_EQ(1234567, d1.count());
}