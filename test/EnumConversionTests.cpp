#include <gtest/gtest.h>

namespace enum_conversion_tests
{
    #include "../src/cpputils/EnumConversion.h"

    enum class TestEnum
    {
        Test0,
        Test1,
        Test2,
        Test3
    };

    DefEnumToStringMap(
        TestEnum,
        { TestEnum::Test0, "Test0" },
        { TestEnum::Test1, "Test1" },
        { TestEnum::Test2, "Test2" },
        { TestEnum::Test3, "Test3" }
    );

    DefStringToEnumMap(
        TestEnum,
        InvariantStringLess,
        { "test0", TestEnum::Test0 },
        { "test1", TestEnum::Test1 },
        { "test2", TestEnum::Test2 },
        { "test3", TestEnum::Test3 }
    );
}

using namespace ::testing;
using namespace ::enum_conversion_tests;
using namespace ::enum_conversion_tests::utl;

TEST(EnumConversionTests, toString)
{
    EXPECT_EQ(std::string("Test0(0)"), utl::EnumConversion<TestEnum>::toString(TestEnum::Test0, true));
    EXPECT_EQ(std::string("Test1"),    utl::EnumConversion<TestEnum>::toString(TestEnum::Test1, false));
    EXPECT_EQ(std::string("Test2(2)"), utl::EnumConversion<TestEnum>::toString(TestEnum::Test2, true));
    EXPECT_EQ(std::string("Test3"),    utl::EnumConversion<TestEnum>::toString(TestEnum::Test3, false));
    EXPECT_EQ(std::string("123"),      utl::EnumConversion<TestEnum>::toString(static_cast<TestEnum>(123), true));
}

TEST(EnumConversionTests, toEnum)
{
    TestEnum e;
    EXPECT_TRUE  (utl::EnumConversion<TestEnum>::tryToEnum("test0", e, false));
    EXPECT_EQ    (TestEnum::Test0, e);

    EXPECT_TRUE  (utl::EnumConversion<TestEnum>::tryToEnum("Test1", e, false));
    EXPECT_EQ    (TestEnum::Test1, e);

    EXPECT_FALSE(utl::EnumConversion<TestEnum>::tryToEnum("asd", e, false));

    EXPECT_FALSE(utl::EnumConversion<TestEnum>::tryToEnum("1", e, false));

    EXPECT_TRUE (utl::EnumConversion<TestEnum>::tryToEnum("2", e, true));
    EXPECT_EQ    (TestEnum::Test2, e);
}