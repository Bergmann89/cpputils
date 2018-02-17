#include <gtest/gtest.h>

#include <cpputils/mp/core.h>

using namespace ::utl::mp;

/* if */
using if_c_expected_0 = int;
using if_c_actual_0   = if_c<true, int, double>;
static_assert(std::is_same<if_c_expected_0, if_c_actual_0>::value, "if");

using if_c_expected_1 = double;
using if_c_actual_1   = if_c<false, int, double>;
static_assert(std::is_same<if_c_expected_1, if_c_actual_1>::value, "if");

/* eval_if(_c) */
using eval_if_expected_0 = int;
using eval_if_actual_0   = eval_if_c<true, int, add_pointer, int, int, int>; /* erroneous add_pointer require only one argument */
static_assert(std::is_same<eval_if_expected_0, eval_if_actual_0>::value, "eval_if(_c)");

using eval_if_expected_1 = int*;
using eval_if_actual_1   = eval_if_c<false, int, add_pointer, int>;
static_assert(std::is_same<eval_if_expected_1, eval_if_actual_1>::value, "");
