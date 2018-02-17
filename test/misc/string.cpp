#include <gtest/gtest.h>
#include <cpputils/misc/string.h>

struct TestClass1
{
    int i;

    std::string to_string() const
        { return std::to_string(i); }

    bool from_string(const std::string& s)
        { return static_cast<bool>(std::istringstream(s) >> i);  }
};

struct TestClass2
{
    int i;

    void to_string(std::ostream& os) const
        { os << i; }

    static bool from_string(const std::string& s, TestClass2& v)
        { return static_cast<bool>(std::istringstream(s) >> v.i);  }
};

using namespace ::utl;
using namespace ::testing;

TEST(StringHelperTests, to_string)
{
    EXPECT_EQ(std::string("4.5"),       utl::to_string(4.5));
    EXPECT_EQ(std::string("test"),      utl::to_string("test"));
    EXPECT_EQ(std::string("fuu"),       utl::to_string(std::string("fuu")));
    EXPECT_EQ(std::string("5"),         utl::to_string(TestClass1 { 5 }));
    EXPECT_EQ(std::string("6"),         utl::to_string(TestClass2 { 6 }));
}

TEST(StringHelperTests, from_string)
{
    double d;
    EXPECT_FALSE(utl::try_from_string("abc", d));
    EXPECT_TRUE (utl::try_from_string("4.5", d));
    EXPECT_TRUE (d >= 4.5 && d <= 4.5);

    int i;
    EXPECT_FALSE(utl::try_from_string("abc", i));
    EXPECT_TRUE (utl::try_from_string("123", i));
    EXPECT_EQ   (123, i);

    TestClass1 t1;
    EXPECT_FALSE(utl::try_from_string("abc", t1));
    EXPECT_TRUE (utl::try_from_string("11",  t1));
    EXPECT_EQ   (11, t1.i);

    TestClass2 t2;
    EXPECT_FALSE(utl::try_from_string("abc", t2));
    EXPECT_TRUE (utl::try_from_string("12",  t2));
    EXPECT_EQ   (12, t2.i);
}