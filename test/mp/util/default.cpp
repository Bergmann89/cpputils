#include <cpputils/mp/util/default.h>

namespace test_mp_util_default
{
    struct is_default_true  : public ::utl::mp::__impl::default_ { };
    struct is_default_false { };
}

using namespace ::utl::mp;
using namespace ::test_mp_util_default;

static_assert( is_default<is_default_true>::value, "");
static_assert(!is_default<is_default_false>::value, "");
static_assert(!is_default<bool>::value, "");