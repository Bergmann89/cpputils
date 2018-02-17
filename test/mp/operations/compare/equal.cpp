#include <cpputils/mp/operations/compare/equal.h>

using namespace ::utl::mp;

namespace test_mp_operations_compare_equal
{
    static_assert(equal(1, 1), "");
    static_assert(!equal(1, 2), "");
}