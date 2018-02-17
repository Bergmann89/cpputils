#include <cpputils/mp/util/tag_of.h>

namespace test_mp_util_tag_of
{
    struct tagged_int   { using tag = int; };
    struct tagged_float { using tag = float; };
    struct tagged_not   { };
}

using namespace ::utl::mp;
using namespace ::test_mp_util_tag_of;

using  tagged_0_expected = int;
using  tagged_0_actual = tag_of<tagged_int>;
static_assert(is_same<tagged_0_expected, tagged_0_actual>::value, "");

using  tagged_1_expected = float;
using  tagged_1_actual = tag_of<tagged_float>;
static_assert(is_same<tagged_1_expected, tagged_1_actual>::value, "");

using  not_tagged_expected = tagged_not;
using  not_tagged_actual = tag_of<tagged_not>;
static_assert(is_same<not_tagged_expected, not_tagged_actual>::value, "");

using bool_expected = bool;
using bool_actual = tag_of<bool>;
static_assert(is_same<bool_expected, bool_actual>::value, "");