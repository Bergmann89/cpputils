#include <cpputils/mp/operations/compare/less.h>

using namespace ::utl::mp;

namespace test_mp_operations_compare_equal
{
    static_assert(less(1, 2), "");
    static_assert(!less(2, 1), "");
}