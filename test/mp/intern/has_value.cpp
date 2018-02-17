#include <cpputils/mp/intern/has_value.h>

using namespace ::utl::mp;
using namespace ::utl::mp::intern;

namespace test_mp_intern_has_value
{
    static_assert( has_value<c_true>::value, "");
    static_assert(!has_value<bool>::value, "");
}