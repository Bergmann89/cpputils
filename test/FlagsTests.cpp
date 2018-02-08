#include <gtest/gtest.h>

namespace flags_tests
{
    #include "../src/cpputils/Flags.h"
}

using namespace ::testing;
using namespace ::flags_tests::utl;

enum class TestFlag : uint8_t
{
    value0,
    value1,
    value2,
    value3,
    value4
};

using TestFlags = ShiftedFlags<TestFlag>;

TEST(FlagsTest, ctor)
{
    TestFlags f0;
    EXPECT_EQ(0, f0());

    TestFlags f1({ TestFlag::value1, TestFlag::value3 });
    EXPECT_EQ(10, f1());

    TestFlags f2(f1);
    EXPECT_EQ(10, f2());

    TestFlags f3(TestFlag::value3);
    EXPECT_EQ(8, f3());
}

TEST(FlagsTest, set)
{
    TestFlags f;
    EXPECT_FALSE(static_cast<bool>(f));
    f.set(TestFlag::value1);
    EXPECT_TRUE(static_cast<bool>(f));
    EXPECT_TRUE(f[TestFlag::value1]);
}

TEST(FlagsTest, isSet)
{
    TestFlags f({ TestFlag::value1, TestFlag::value3 });

    EXPECT_FALSE(f[TestFlag::value0]);
    EXPECT_TRUE (f[TestFlag::value1]);
    EXPECT_FALSE(f[TestFlag::value2]);
    EXPECT_TRUE (f[TestFlag::value3]);
    EXPECT_FALSE(f[TestFlag::value4]);

    EXPECT_FALSE(f.isSet(TestFlag::value0));
    EXPECT_TRUE (f.isSet(TestFlag::value1));
    EXPECT_FALSE(f.isSet(TestFlag::value2));
    EXPECT_TRUE (f.isSet(TestFlag::value3));
    EXPECT_FALSE(f.isSet(TestFlag::value4));
}

TEST(FlagsTest, clear)
{
    TestFlags f({ TestFlag::value1, TestFlag::value3 });
    f.clear(TestFlag::value1);
    EXPECT_FALSE(f.isSet(TestFlag::value1));
    EXPECT_TRUE (f.isSet(TestFlag::value3));
}

TEST(FlagsTest, reset)
{
    TestFlags f({ TestFlag::value1, TestFlag::value3 });
    f.reset();
    EXPECT_FALSE(static_cast<bool>(f));
}