#include <gtest/gtest.h>
#include <cpputils/misc/misc.h>

using namespace ::utl;
using namespace ::testing;

TEST(UtilsTests, bit_count)
{
    EXPECT_EQ(0, bit_count(0b0000'0000'0000'0000'0000'0000'0000'0000u));
    EXPECT_EQ(1, bit_count(0b0000'0000'0000'0000'0000'0000'0000'1000u));
    EXPECT_EQ(2, bit_count(0b0000'0000'0000'0000'0000'1000'0000'1000u));
    EXPECT_EQ(3, bit_count(0b0000'0000'0010'0000'0000'1000'0000'1000u));
    EXPECT_EQ(4, bit_count(0b0010'0000'0010'0000'0000'1000'0000'1000u));
    EXPECT_EQ(5, bit_count(0b0010'0000'0010'0000'0000'1000'1000'1000u));
    EXPECT_EQ(6, bit_count(0b0010'0000'0010'0000'0000'1000'1000'1001u));
    EXPECT_EQ(7, bit_count(0b0010'0000'0010'0000'0000'1000'1000'1101u));
    EXPECT_EQ(8, bit_count(0b0010'0100'0010'0000'0000'1000'1000'1101u));
}