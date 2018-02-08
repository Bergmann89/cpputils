#include <gtest/gtest.h>

namespace utils_tests
{
    #include "../src/cpputils/Misc.h"
}

using namespace ::testing;
using namespace ::utils_tests::utl;

TEST(UtilsTests, bitCount)
{
    EXPECT_EQ(0, bitCount(0b0000'0000'0000'0000'0000'0000'0000'0000u));
    EXPECT_EQ(1, bitCount(0b0000'0000'0000'0000'0000'0000'0000'1000u));
    EXPECT_EQ(2, bitCount(0b0000'0000'0000'0000'0000'1000'0000'1000u));
    EXPECT_EQ(3, bitCount(0b0000'0000'0010'0000'0000'1000'0000'1000u));
    EXPECT_EQ(4, bitCount(0b0010'0000'0010'0000'0000'1000'0000'1000u));
    EXPECT_EQ(5, bitCount(0b0010'0000'0010'0000'0000'1000'1000'1000u));
    EXPECT_EQ(6, bitCount(0b0010'0000'0010'0000'0000'1000'1000'1001u));
    EXPECT_EQ(7, bitCount(0b0010'0000'0010'0000'0000'1000'1000'1101u));
    EXPECT_EQ(8, bitCount(0b0010'0100'0010'0000'0000'1000'1000'1101u));
}