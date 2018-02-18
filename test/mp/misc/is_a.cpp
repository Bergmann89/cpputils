#include <cstdint>
#include <cpputils/mp/core/const.h>
#include <cpputils/mp/misc/is_a.h>

namespace test_mp_util_is_a
{
    struct tagged_int   { using tag = int; };
    struct tagged_float { using tag = float; };
    struct tagged_not   { };
}

using namespace ::utl::mp;
using namespace ::test_mp_util_is_a;

static_assert( is_a<int, tagged_int>, "is_a");
static_assert(!is_a<int, tagged_float>, "is_a");
static_assert( is_a<tag_integral_constant<int>>(integral_constant<int, 1> { }), "is_a");
static_assert(!is_a<tag_integral_constant<int>>(integral_constant<uint8_t, 1> { }), "is_a");