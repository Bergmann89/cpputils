#include <memory>
#include <type_traits>
#include <gtest/gtest.h>

namespace string_helper_tests
{
    #include "../src/cpputils/StringHelper.h"

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

    struct TestClass1
    {
        int i;

        std::string toString() const
            { return std::to_string(i); }

        bool fromString(const std::string& s)
            { return static_cast<bool>(std::istringstream(s) >> i);  }
    };

    struct TestClass2
    {
        int i;

        void toString(std::ostream& os) const
            { os << i; }

        static bool fromString(const std::string& s, TestClass2& v)
            { return static_cast<bool>(std::istringstream(s) >> v.i);  }
    };
}

using namespace ::testing;
using namespace ::string_helper_tests;
using namespace ::string_helper_tests::utl;

TEST(StringHelperTests, toString)
{
    EXPECT_EQ(std::string("4.5"),       utl::toString(4.5));
    EXPECT_EQ(std::string("test"),      utl::toString("test"));
    EXPECT_EQ(std::string("fuu"),       utl::toString(std::string("fuu")));
    EXPECT_EQ(std::string("Test1(1)"),  utl::toString(TestEnum::Test1));
    EXPECT_EQ(std::string("5"),         utl::toString(TestClass1 { 5 }));
    EXPECT_EQ(std::string("6"),         utl::toString(TestClass2 { 6 }));
}

TEST(StringHelperTests, fromString)
{
    double d;
    EXPECT_FALSE(utl::tryFromString("abc", d));
    EXPECT_TRUE (utl::tryFromString("4.5", d));
    EXPECT_TRUE (d >= 4.5 && d <= 4.5);

    int i;
    EXPECT_FALSE(utl::tryFromString("abc", i));
    EXPECT_TRUE (utl::tryFromString("123", i));
    EXPECT_EQ   (123, i);

    TestEnum e;
    EXPECT_FALSE(utl::tryFromString("abc",   e));
    EXPECT_TRUE (utl::tryFromString("test1", e));
    EXPECT_EQ   (TestEnum::Test1, e);

    TestClass1 t1;
    EXPECT_FALSE(utl::tryFromString("abc", t1));
    EXPECT_TRUE (utl::tryFromString("11",  t1));
    EXPECT_EQ   (11, t1.i);

    TestClass2 t2;
    EXPECT_FALSE(utl::tryFromString("abc", t2));
    EXPECT_TRUE (utl::tryFromString("12",  t2));
    EXPECT_EQ   (12, t2.i);
}