#include <cpputils/mp/container/pair.h>
#include <cpputils/mp/operations/compare/equal.h>
#include <cpputils/mp/operations/compare/not_equal.h>
#include <cpputils/mp/operations/compare/less.h>
#include <cpputils/mp/operations/compare/less_equal.h>
#include <cpputils/mp/operations/compare/greater.h>
#include <cpputils/mp/operations/compare/greater_equal.h>

using namespace ::utl::mp;
using namespace ::utl::mp::operators;

namespace test_mp_container_pair
{
    constexpr auto my_pair = make_pair(1, 'x');
    static_assert(first(my_pair) == 1, "");
    static_assert(second(my_pair) == 'x', "");
    static_assert(my_pair.first() == 1, "");
    static_assert(my_pair.second() == 'x', "");

    static_assert(equal(make_pair(1, 'x'),   make_pair(1, 'x')), "");
    static_assert(      make_pair(1, 'x') == make_pair(1, 'x'), "");

    static_assert(not_equal(make_pair(1, 'x'),   make_pair(2, 'x')), "");
    static_assert(          make_pair(1, 'x') != make_pair(2, 'x'), "");

    static_assert(less(make_pair(1, 'x'), make_pair(1, 'y')), "");
    static_assert(less(make_pair(1, 'x'), make_pair(2, 'x')), "");
    static_assert(less(make_pair(1, 'x'), make_pair(2, 'a')), "");

    static_assert(make_pair(1, 'x') < make_pair(1, 'y'), "");
    static_assert(make_pair(1, 'x') < make_pair(2, 'x'), "");
    static_assert(make_pair(1, 'x') < make_pair(2, 'a'), "");

    static_assert(less_equal(make_pair(1, 'x'), make_pair(1, 'x')), "");
    static_assert(less_equal(make_pair(1, 'x'), make_pair(1, 'y')), "");
    static_assert(less_equal(make_pair(1, 'x'), make_pair(2, 'x')), "");
    static_assert(less_equal(make_pair(1, 'x'), make_pair(2, 'a')), "");

    static_assert(make_pair(1, 'x') <= make_pair(1, 'x'), "");
    static_assert(make_pair(1, 'x') <= make_pair(1, 'y'), "");
    static_assert(make_pair(1, 'x') <= make_pair(2, 'x'), "");
    static_assert(make_pair(1, 'x') <= make_pair(2, 'a'), "");

    static_assert(greater(make_pair(1, 'y'), make_pair(1, 'x')), "");
    static_assert(greater(make_pair(2, 'x'), make_pair(1, 'x')), "");
    static_assert(greater(make_pair(2, 'a'), make_pair(1, 'x')), "");

    static_assert(make_pair(1, 'y') > make_pair(1, 'x'), "");
    static_assert(make_pair(2, 'x') > make_pair(1, 'x'), "");
    static_assert(make_pair(2, 'a') > make_pair(1, 'x'), "");

    static_assert(greater_equal(make_pair(1, 'x'), make_pair(1, 'x')), "");
    static_assert(greater_equal(make_pair(1, 'y'), make_pair(1, 'x')), "");
    static_assert(greater_equal(make_pair(2, 'x'), make_pair(1, 'x')), "");
    static_assert(greater_equal(make_pair(2, 'a'), make_pair(1, 'x')), "");

    static_assert(make_pair(1, 'x') >= make_pair(1, 'x'), "");
    static_assert(make_pair(1, 'y') >= make_pair(1, 'x'), "");
    static_assert(make_pair(2, 'x') >= make_pair(1, 'x'), "");
    static_assert(make_pair(2, 'a') >= make_pair(1, 'x'), "");
}