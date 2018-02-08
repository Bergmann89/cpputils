#include <memory>
#include <type_traits>
#include <gtest/gtest.h>

namespace nullable_tests
{
    #include "../src/cpputils/Nullable.h"

    struct TestData
    {
        static int ctorCount;
        static int dtorCount;

        TestData()
            { ++ctorCount; }

         ~TestData()
            { ++dtorCount; }
    };

    struct NonCopyableTestData
    {
        int value;

        NonCopyableTestData(int v) :
            value(v)
            { }

        NonCopyableTestData(NonCopyableTestData&& other) :
            value(0)
            { std::swap(value, other.value); }

        NonCopyableTestData(const NonCopyableTestData&) = delete;
    };

    using NullableInt                 = utl::Nullable<int>;
    using NullableIntRef              = utl::Nullable<int&>;
    using NullableString              = utl::Nullable<std::string>;
    using NullableTestData            = utl::Nullable<TestData>;
    using NullableNonCopyableTestData = utl::Nullable<NonCopyableTestData>;

    int TestData::ctorCount = 0;
    int TestData::dtorCount = 0;
}

using namespace nullable_tests;
using namespace nullable_tests::utl;

TEST(NullableTest, ctor_empty)
{
    NullableInt    n1;
    NullableIntRef n2;
    EXPECT_FALSE(static_cast<bool>(n1));
    EXPECT_FALSE(static_cast<bool>(n2));
}

TEST(NullableTest, ctor_value)
{
    int i = 5;
    NullableInt    n1(i);
    NullableIntRef n2(i);
    EXPECT_TRUE(static_cast<bool>(n1));
    EXPECT_TRUE(static_cast<bool>(n2));
    EXPECT_EQ  (5,   n1());
    EXPECT_EQ  (&i, &n2());
}

TEST(NullableTest, ctor_copy)
{
    int i = 5;
    NullableInt    i1(i);
    NullableIntRef i2(i);
    NullableInt    n1(i1);
    NullableIntRef n2(i2);
    EXPECT_TRUE(static_cast<bool>(n1));
    EXPECT_TRUE(static_cast<bool>(n2));
    EXPECT_EQ  (5,   n1());
    EXPECT_EQ  (&i, &n2());
}

TEST(NullableTest, ctor_move)
{
    NullableString i1("test");
    NullableString n1(std::move(i1));
    EXPECT_FALSE(static_cast<bool>(i1));
    ASSERT_TRUE (static_cast<bool>(n1));
    EXPECT_EQ   (std::string("test"), n1());
}

TEST(NullableTest, move_assignment)
{
    NullableString i1("test");
    NullableString n1;
    n1 = std::move(i1);
    EXPECT_FALSE(static_cast<bool>(i1));
    ASSERT_TRUE (static_cast<bool>(n1));
    EXPECT_EQ   (std::string("test"), n1());
}

TEST(NullableTest, movable_object)
{
    NonCopyableTestData data(5);
    NullableNonCopyableTestData tmp;
    tmp = std::move(data);
    ASSERT_TRUE ( tmp.hasValue() );
    ASSERT_EQ   ( 5, tmp.value().value );
    ASSERT_EQ   ( 0, data.value );
}

TEST(NullableTest, hasValue_operatorBool)
{
    EXPECT_FALSE(static_cast<bool>(NullableInt()));
    EXPECT_FALSE(NullableInt().hasValue());
    EXPECT_TRUE (static_cast<bool>(NullableInt(5)));
    EXPECT_TRUE (NullableInt(5).hasValue());
}

TEST(NullableTest, reset)
{
    NullableTestData n(TestData{});
    EXPECT_TRUE (n.hasValue());
    int tmp = TestData::dtorCount;
    n.reset();
    EXPECT_FALSE(n.hasValue());
    EXPECT_EQ   (tmp + 1, TestData::dtorCount);
}

TEST(NullableTest, value_functor)
{
    NullableInt n1(5);
    NullableInt n2;

    EXPECT_EQ       (5, n1());
    EXPECT_EQ       (5, n1.value());
    EXPECT_ANY_THROW(n2());
    EXPECT_ANY_THROW(n2.value());
}

TEST(NullableTest, equalityCompareOperator)
{
    NullableInt n1(5);
    NullableInt n2(7);
    NullableInt n3(5);
    NullableInt n4;

    EXPECT_FALSE(n2 == n1);
    EXPECT_TRUE (n3 == n1);
    EXPECT_FALSE(n4 == n1);

    EXPECT_FALSE(n1 == n2);
    EXPECT_FALSE(n3 == n2);
    EXPECT_FALSE(n4 == n2);

    EXPECT_TRUE (n1 == n3);
    EXPECT_FALSE(n2 == n3);
    EXPECT_FALSE(n4 == n3);

    EXPECT_FALSE(n1 == n4);
    EXPECT_FALSE(n2 == n4);
    EXPECT_FALSE(n3 == n4);
}
