#include <gtest/gtest.h>
#include <cpputils/mp/intern/ebo.h>

using namespace ::utl::mp::intern;

struct Key { };
using ebo_type = ebo<Key, int>;
constexpr ebo_type myEbo(7);
static_assert(myEbo.get() == 7, "ebo.get");