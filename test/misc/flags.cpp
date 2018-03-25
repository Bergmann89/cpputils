#include <gtest/gtest.h>
#include <cpputils/misc/flags.h>

using namespace ::utl;
using namespace ::testing;

enum class TestFlag : uint8_t
{
    value0,
    value1,
    value2,
    value3,
    value4
};

using Testflags = shifted_flags<TestFlag>;

TEST(flagsTest, ctor)
{
    Testflags f0;
    EXPECT_EQ(0, f0());

    Testflags f1({ TestFlag::value1, TestFlag::value3 });
    EXPECT_EQ(10, f1());

    Testflags f2(f1);
    EXPECT_EQ(10, f2());

    Testflags f3(TestFlag::value3);
    EXPECT_EQ(8, f3());
}

TEST(flagsTest, set)
{
    Testflags f;
    EXPECT_FALSE(static_cast<bool>(f));
    f.set(TestFlag::value1);
    EXPECT_TRUE(static_cast<bool>(f));
    EXPECT_TRUE(f[TestFlag::value1]);
}

TEST(flagsTest, is_set)
{
    Testflags f({ TestFlag::value1, TestFlag::value3 });

    EXPECT_FALSE(f[TestFlag::value0]);
    EXPECT_TRUE (f[TestFlag::value1]);
    EXPECT_FALSE(f[TestFlag::value2]);
    EXPECT_TRUE (f[TestFlag::value3]);
    EXPECT_FALSE(f[TestFlag::value4]);

    EXPECT_FALSE(f.is_set(TestFlag::value0));
    EXPECT_TRUE (f.is_set(TestFlag::value1));
    EXPECT_FALSE(f.is_set(TestFlag::value2));
    EXPECT_TRUE (f.is_set(TestFlag::value3));
    EXPECT_FALSE(f.is_set(TestFlag::value4));
}

TEST(flagsTest, clear)
{
    Testflags f({ TestFlag::value1, TestFlag::value3 });
    f.clear(TestFlag::value1);
    EXPECT_FALSE(f.is_set(TestFlag::value1));
    EXPECT_TRUE (f.is_set(TestFlag::value3));
}

TEST(flagsTest, reset)
{
    Testflags f({ TestFlag::value1, TestFlag::value3 });
    f.reset();
    EXPECT_FALSE(static_cast<bool>(f));
}

TEST(flagsTest, iterator)
{
    Testflags f({ TestFlag::value1, TestFlag::value3 });
    auto it  = f.begin();
    auto end = f.end();

    EXPECT_FALSE(it == end);
    EXPECT_TRUE (it != end);
    EXPECT_EQ   (TestFlag::value1, *it);
    ++it;

    EXPECT_FALSE(it == end);
    EXPECT_TRUE (it != end);
    EXPECT_EQ   (TestFlag::value3, *it);
    ++it;

    EXPECT_TRUE (it == end);
    EXPECT_FALSE(it != end);
    ++it;

    EXPECT_TRUE (it == end);
    EXPECT_FALSE(it != end);
}