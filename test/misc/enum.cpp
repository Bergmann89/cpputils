#include <gtest/gtest.h>
#include <cpputils/misc/enum.h>

enum class TestEnum
{
    Test0,
    Test1,
    Test2,
    Test3
};

DEFINE_ENUM_TO_STRING_MAP(
    TestEnum,
    { TestEnum::Test0, "Test0" },
    { TestEnum::Test1, "Test1" },
    { TestEnum::Test2, "Test2" },
    { TestEnum::Test3, "Test3" }
);

DEFINE_STRING_TO_ENUM_MAP(
    TestEnum,
    invariant_string_less,
    { "test0", TestEnum::Test0 },
    { "test1", TestEnum::Test1 },
    { "test2", TestEnum::Test2 },
    { "test3", TestEnum::Test3 }
);

using namespace ::utl;
using namespace ::testing;

TEST(enum_tests, to_string)
{
    EXPECT_EQ(std::string("Test1(1)"), utl::to_string(TestEnum::Test1));
    EXPECT_EQ(std::string("Test0(0)"), utl::enum_conversion<TestEnum>::to_string(TestEnum::Test0, true));
    EXPECT_EQ(std::string("Test1"),    utl::enum_conversion<TestEnum>::to_string(TestEnum::Test1, false));
    EXPECT_EQ(std::string("Test2(2)"), utl::enum_conversion<TestEnum>::to_string(TestEnum::Test2, true));
    EXPECT_EQ(std::string("Test3"),    utl::enum_conversion<TestEnum>::to_string(TestEnum::Test3, false));
    EXPECT_EQ(std::string("123"),      utl::enum_conversion<TestEnum>::to_string(static_cast<TestEnum>(123), true));
}

TEST(enum_tests, from_string)
{
    TestEnum e;
    EXPECT_FALSE(utl::try_from_string("abc",   e));
    EXPECT_TRUE (utl::try_from_string("test1", e));
    EXPECT_EQ   (TestEnum::Test1, e);
}

TEST(enum_tests, to_enum)
{
    TestEnum e;
    EXPECT_TRUE  (utl::enum_conversion<TestEnum>::try_to_enum("test0", e, false));
    EXPECT_EQ    (TestEnum::Test0, e);

    EXPECT_TRUE  (utl::enum_conversion<TestEnum>::try_to_enum("Test1", e, false));
    EXPECT_EQ    (TestEnum::Test1, e);

    EXPECT_FALSE(utl::enum_conversion<TestEnum>::try_to_enum("asd", e, false));

    EXPECT_FALSE(utl::enum_conversion<TestEnum>::try_to_enum("1", e, false));

    EXPECT_TRUE (utl::enum_conversion<TestEnum>::try_to_enum("2", e, true));
    EXPECT_EQ    (TestEnum::Test2, e);
}