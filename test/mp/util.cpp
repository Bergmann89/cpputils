#include <gtest/gtest.h>
#include "../../src/cpputils/mp/util.h"

using namespace ::utl::mp;

struct tagged_int { using tag = int; };
struct tagged_float { using tag = float; };
struct tagged_not { };

/* default */
struct is_default_true  : public __impl::default_ { };
struct is_default_false { };
static_assert(is_default<is_default_true>::value,   "is_default");
static_assert(!is_default<is_default_false>::value, "is_default");

/* tag_of */
using  tagged_0_expected = int;
using  tagged_1_expected = float;
using  tagged_0_actual = tag_of<tagged_int>;
using  tagged_1_actual = tag_of<tagged_float>;
using  not_tagged_expected = tagged_not;
using  not_tagged_actual = tag_of<tagged_not>;
static_assert(is_same<tagged_0_expected, tagged_0_actual>::value, "tag_of");
static_assert(is_same<tagged_1_expected, tagged_1_actual>::value, "tag_of");
static_assert(is_same<not_tagged_expected, not_tagged_actual>::value, "tag_of");

/* is_a */
static_assert(is_a<int, tagged_int>, "is_a");
static_assert(!is_a<int, tagged_float>, "is_a");
static_assert(is_a<tag_const<int>>(make_const<int, 1>), "is_a");
static_assert(!is_a<tag_const<int>>(make_const<uint8_t, 1>), "is_a");

TEST(mp_util_test, make)
{
    EXPECT_EQ(std::string("fuu"), make<std::string>("fuu"));
}