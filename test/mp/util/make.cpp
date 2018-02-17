#include <gtest/gtest.h>
#include <cpputils/mp/util/make.h>

using namespace ::utl::mp;

TEST(mp_util_test, make)
{
    EXPECT_EQ(std::string("fuu"), make<std::string>("fuu"));
}