#include <cpputils/mp/intern/comparable_equal.h>

using namespace ::utl::mp::intern;

namespace test_mp_intern_comparable_equal
{
    static_assert(comparable_equal<bool, bool>::value, "");
}
