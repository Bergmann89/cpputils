#include <cpputils/mp/operations/value.h>

using namespace ::utl::mp;

namespace test_mp_operations_value
{
    constexpr integral_constant<int, 6> my_const{ };
    static_assert(value(my_const) == 6, "");
}