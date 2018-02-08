#include <gtest/gtest.h>

#include "../src/cpputils/MetaProgramming.h"

using namespace utl;

struct X1 { };
struct X2 { };
struct X3 { };

struct Y1 { };
struct Y2 { };
struct Y3 { };

struct Z1 { };
struct Z2 { };
struct Z3 { };


/* logical operations ****************************************************************************/
/* mp_if */
using mp_if_c_expected_0 = int;
using mp_if_c_actual_0   = mp_if_c<true, int, double>;
static_assert(std::is_same<mp_if_c_expected_0, mp_if_c_actual_0>::value, "mp_if");

using mp_if_c_expected_1 = double;
using mp_if_c_actual_1   = mp_if_c<false, int, double>;
static_assert(std::is_same<mp_if_c_expected_1, mp_if_c_actual_1>::value, "mp_if");

/* mp_eval_if(_c) */
using mp_eval_if_expected_0 = int;
using mp_eval_if_actual_0   = mp_eval_if_c<true, int, mp_add_pointer, int, int, int>; /* erroneous mp_add_pointer require only one argument */
static_assert(std::is_same<mp_eval_if_expected_0, mp_eval_if_actual_0>::value, "mp_eval_if(_c)");

using mp_eval_if_expected_1 = int*;
using mp_eval_if_actual_1   = mp_eval_if_c<false, int, mp_add_pointer, int>;
static_assert(std::is_same<mp_eval_if_expected_1, mp_eval_if_actual_1>::value, "");

/* mp_not */
using mp_not_input   = mp_list<mp_false, mp_true, mp_false>;
using mp_not_exected = mp_list<mp_true, mp_false, mp_true>;
using mp_not_actual  = mp_not<mp_not_input>;
static_assert(std::is_same<mp_not_exected, mp_not_actual>::value, "mp_not");

/* mp_and */
using mp_and_input_0    = mp_list<mp_true, mp_false, mp_true>;
using mp_and_expected_0 = mp_false;
using mp_and_actual_0   = mp_and<mp_and_input_0>;
static_assert(std::is_same<mp_and_expected_0, mp_and_actual_0>::value, "mp_and");

using mp_and_input_1    = mp_list<mp_true, mp_true, mp_true>;
using mp_and_expected_1 = mp_true;
using mp_and_actual_1   = mp_and<mp_and_input_1>;
static_assert(std::is_same<mp_and_expected_1, mp_and_actual_1>::value, "mp_and");

/* mp_or */
using mp_or_input_0    = mp_list<mp_false, mp_false, mp_false>;
using mp_or_expected_0 = mp_false;
using mp_or_actual_0   = mp_or<mp_or_input_0>;
static_assert(std::is_same<mp_or_expected_0, mp_or_actual_0>::value, "mp_or");

using mp_or_input_1    = mp_list<mp_false, mp_false, mp_true>;
using mp_or_expected_1 = mp_true;
using mp_or_actual_1   = mp_or<mp_or_input_1>;
static_assert(std::is_same<mp_or_expected_1, mp_or_actual_1>::value, "mp_or");



/* arithmetic ************************************************************************************/
/* mp_plus */
using mp_plus_input    = mp_list<mp_const<int, 1>, mp_const<uint, 5>, mp_const<uint8_t, 3>>;
using mp_plus_expected = mp_const<int, 9>;
using mp_plus_actual   = mp_plus<mp_plus_input>;
static_assert(std::is_same<mp_plus_expected, mp_plus_actual>::value, "mp_plus");

/* mp_minus */
using mp_minus_input    = mp_list<mp_const<int, 1>, mp_const<uint, 5>, mp_const<uint8_t, 3>>;
using mp_minus_expected = mp_const<int, -7>;
using mp_minus_actual   = mp_minus<mp_minus_input>;
static_assert(std::is_same<mp_minus_expected, mp_minus_actual>::value, "mp_minus");

/* mp_multiply */
using mp_multiply_input    = mp_list<mp_const<int, 1>, mp_const<uint, 5>, mp_const<uint8_t, 3>>;
using mp_multiply_expected = mp_const<int, 15>;
using mp_multiply_actual   = mp_multiply<mp_multiply_input>;
static_assert(std::is_same<mp_multiply_expected, mp_multiply_actual>::value, "mp_multiply");

/* mp_divide */
using mp_divide_input    = mp_list<mp_const<int, 75>, mp_const<uint, 5>, mp_const<uint8_t, 3>>;
using mp_divide_expected = mp_const<int, 5>;
using mp_divide_actual   = mp_divide<mp_divide_input>;
static_assert(std::is_same<mp_divide_expected, mp_divide_actual>::value, "mp_divide");



/* generator functions ***************************************************************************/
/* mp_make_integer_sequence */
using mp_make_integer_sequence_expected = mp_integer_sequence<int, 0, 1, 2, 3, 4, 5>;
using mp_make_integer_sequence_actual   = mp_make_integer_sequence<int, 6>;
static_assert(std::is_same<mp_make_integer_sequence_expected, mp_make_integer_sequence_actual>::value, "mp_make_integer_sequence");

/* mp_make_index_sequence */
using mp_make_index_sequence_expected = mp_index_sequence<0, 1, 2, 3, 4, 5>;
using mp_make_index_sequence_actual   = mp_make_index_sequence<6>;
static_assert(std::is_same<mp_make_index_sequence_expected, mp_make_index_sequence_actual>::value, "mp_make_index_sequence");

/* mp_repeat */
using mp_repeat_expected = mp_list<X1, X2, X3, X1, X2, X3, X1, X2, X3>;
using mp_repeat_actual   = mp_repeat<3, X1, X2, X3>;
static_assert(std::is_same<mp_repeat_expected, mp_repeat_actual>::value, "mp_repeat");



/* list operations returning a numeric value *****************************************************/
/* mp_length */
using mp_length_expected = mp_size_t<3>;
using mp_length_actual   = mp_length<X1, X2, X3>;
static_assert(std::is_same<mp_length_expected, mp_length_actual>::value, "mp_length");

/* mp_size */
using mp_size_input     = mp_list<X1, X2, X3>;
using mp_size_expected  = mp_size_t<3>;
using mp_size_actual    = mp_size<mp_size_input>;
static_assert(std::is_same<mp_size_expected, mp_size_actual>::value, "mp_size");

/* mp_count */
using mp_count_input    = mp_list<X1, X2, X3, X1, X2, X1>;
using mp_count_expected = mp_size_t<3>;
using mp_count_actual   = mp_count<mp_count_input, X1>;
static_assert(std::is_same<mp_count_expected, mp_count_actual>::value, "mp_count");

/* mp_count_if */
template<class T>
using mp_count_if_op       = mp_or<mp_list<std::is_same<T, X1>, std::is_same<T, X2>>>;
using mp_count_if_input    = mp_list<X1, X2, X3, X1, X2, X1>;
using mp_count_if_expected = mp_size_t<5>;
using mp_count_if_actual   = mp_count_if<mp_count_input, mp_count_if_op>;
static_assert(std::is_same<mp_count_if_expected, mp_count_if_actual>::value, "mp_count_if");



/* operations to manipulate list content *********************************************************/
/* mp_push_front */
using mp_push_front_input    = mp_list<X1, X2>;
using mp_push_front_expected = mp_list<Y1, Y2, X1, X2>;
using mp_push_front_actual   = mp_push_front<mp_push_front_input, Y1, Y2>;
static_assert(std::is_same<mp_push_front_expected, mp_push_front_actual>::value, "mp_push_front");

/* mp_pop_front */
using mp_pop_front_input    = mp_list<X1, X2, X3>;
using mp_pop_front_expected = mp_list<X2, X3>;
using mp_pop_front_actual   = mp_pop_front<mp_pop_front_input>;
static_assert(std::is_same<mp_pop_front_expected, mp_pop_front_actual>::value, "mp_pop_front");

/* mp_push_bask */
using mp_push_back_input    = mp_list<X1, X2>;
using mp_push_back_expected = mp_list<X1, X2, Y1, Y2>;
using mp_push_back_actual   = mp_push_back<mp_push_back_input, Y1, Y2>;
static_assert(std::is_same<mp_push_back_expected, mp_push_back_actual>::value, "mp_push_bask");

/* mp_concat */
using mp_concat_expected = mp_list<X1, X2, Y1, Y2, Z1, Z2, Z3>;
using mp_concat_actual   = mp_concat<mp_list<X1, X2>, mp_list<Y1, Y2>, mp_list<Z1, Z2, Z3>>;
static_assert(std::is_same<mp_concat_expected, mp_concat_actual>::value, "mp_concat");

/* mp_filter */
template<class T>
using mp_filter_func     = mp_bool<mp_or<mp_list<mp_is_same<T, X2>, mp_is_same<T, Y2>, mp_is_same<T, Z2>>>::value == 0>;
using mp_filter_input    = mp_list<X1, X2, X3, Y1, Y2, Y3, Z1, Z2, Z3>;
using mp_filter_expected = mp_list<X1, X3, Y1, Y3, Z1, Z3>;
using mp_filter_actual   = mp_filter<mp_filter_input, mp_filter_func>;
static_assert(std::is_same<mp_filter_expected, mp_filter_actual>::value, "mp_filter");



/* list operations returning a bool value ********************************************************/
/* mp_empty */
using mp_empty_input_0    = mp_list<>;
using mp_empty_input_1    = mp_list<X1>;
using mp_empty_expected_0 = mp_false;
using mp_empty_actual_0   = mp_empty<mp_empty_input_0, mp_empty_input_1>;
static_assert(std::is_same<mp_empty_expected_0, mp_empty_actual_0>::value, "mp_empty");

using mp_empty_expected_1 = mp_true;
using mp_empty_actual_1   = mp_empty<mp_empty_input_0, mp_empty_input_0>;
static_assert(std::is_same<mp_empty_expected_1, mp_empty_actual_1>::value, "mp_empty");

/* mp_contains */
using mp_contains_input_0    = mp_list<X1, X2, X3, X1>;
using mp_contains_expected_0 = mp_true;
using mp_contains_actual_0   = mp_contains<mp_contains_input_0, X1>;
static_assert(std::is_same<mp_contains_expected_0, mp_contains_actual_0>::value, "mp_contains");

using mp_contains_input_1    = mp_list<X1, X2, X3>;
using mp_contains_expected_1 = mp_false;
using mp_contains_actual_1   = mp_contains<mp_contains_input_1, Y1>;
static_assert(std::is_same<mp_contains_expected_1, mp_contains_actual_1>::value, "mp_contains");



/* list operations returing elements from the list ***********************************************/
/* mp_front */
using mp_front_input    = mp_list<X1, X2, X3>;
using mp_front_expected = X1;
using mp_front_actual   = mp_front<mp_front_input>;
static_assert(std::is_same<mp_front_expected, mp_front_actual>::value, "mp_front");

/* mp_second */
using mp_second_input    = mp_list<X1, X2, X3>;
using mp_second_expected = X2;
using mp_second_actual   = mp_second<mp_second_input>;
static_assert(std::is_same<mp_second_expected, mp_second_actual>::value, "mp_second");

/* mp_at(_c) */
using mp_at_c_input    = mp_list<X1, X2, X3, Y1, Y2, Y3>;
using mp_at_c_expected = Y1;
using mp_at_c_actual   = mp_at_c<mp_at_c_input, 3>;
static_assert(std::is_same<mp_at_c_expected, mp_at_c_actual>::value, "mp_at(_c)");

/* mp_search */
template<class T>
using mp_search_func_0     = std::is_same<T, X2>;
using mp_search_input_0    = mp_list<X1, X2, X3>;
using mp_search_expected_0 = X2;
using mp_search_actual_0   = mp_search<mp_search_input_0, mp_search_func_0>;
static_assert(std::is_same<mp_search_expected_0, mp_search_actual_0>::value, "mp_search");

template<class T>
using mp_search_func_1     = mp_false;
using mp_search_input_1    = mp_list<X1, X2, X3>;
using mp_search_expected_1 = Y2;
using mp_search_actual_1   = mp_search<mp_search_input_1, mp_search_func_1, Y2>;
static_assert(std::is_same<mp_search_expected_1, mp_search_actual_1>::value, "mp_search");

/* mp_find */
using mp_find_input_0    = mp_list<X1, X2, X3, Y2>;
using mp_find_expected_0 = mp_size_t<2>;
using mp_find_actual_0   = mp_find<mp_find_input_0, X3>;
static_assert(std::is_same<mp_find_expected_0, mp_find_actual_0>::value, "mp_find");

using mp_find_input_1    = mp_list<X1, X2, X3, Y2>;
using mp_find_expected_1 = mp_size_t<4>;
using mp_find_actual_1   = mp_find<mp_find_input_0, Z1>;
static_assert(std::is_same<mp_find_expected_1, mp_find_actual_1>::value, "mp_find");

/* mp_map_find */
using mp_map_find_input_0    = mp_list<mp_list<X1, X2, X3>, mp_list<Y1, Y2, Y3>, mp_list<Z1, Z2, Z3>>;
using mp_map_find_expected_0 = mp_list<Y1, Y2, Y3>;
using mp_map_find_actual_0   = mp_map_find<mp_map_find_input_0, Y1>;
static_assert(std::is_same<mp_map_find_expected_0, mp_map_find_actual_0>::value, "mp_map_find");

using mp_map_find_input_1    = mp_list<mp_list<X1, X2, X3>, mp_list<Y1, Y2, Y3>, mp_list<Z1, Z2, Z3>>;
using mp_map_find_expected_1 = void;
using mp_map_find_actual_1   = mp_map_find<mp_map_find_input_1, X3>;
static_assert(std::is_same<mp_map_find_expected_1, mp_map_find_actual_1>::value, "mp_map_find");



/* list manipulators returning a new list ********************************************************/
/* mp_clear */
using mp_clear_input    = mp_list<X1, X2, X3>;
using mp_clear_expected = mp_list<>;
using mp_clear_actual   = mp_clear<mp_clear_input>;
static_assert(std::is_same<mp_clear_expected, mp_clear_actual>::value, "mp_unique");

/* mp_unique */
using mp_unique_input    = mp_list<X1, X2, X1, X2, X3, X1, X2, Y1, X3>;
using mp_unique_expected = mp_list<X1, X2, X3, Y1>;
using mp_unique_actual   = mp_unique<mp_unique_input>;
static_assert(std::is_same<mp_unique_expected, mp_unique_actual>::value, "mp_unique");

/* mp_rename */
using mp_rename_input    = mp_list<X1, X2, X3>;
using mp_rename_expected = std::tuple<X1, X2, X3>;
using mp_rename_actual   = mp_rename<mp_rename_input, std::tuple>;
static_assert(std::is_same<mp_rename_expected, mp_rename_actual>::value, "mp_rename");

/* mp_append */
using mp_append_input_X  = mp_list<X1, X2, X3>;
using mp_append_input_Y  = mp_list<Y1, Y2, Y3>;
using mp_append_input_Z  = mp_list<Z1, Z2, Z3>;
using mp_append_expected = mp_list<X1, X2, X3, Y1, Y2, Y3, Z1, Z2, Z3>;
using mp_append_actual   = mp_append<mp_append_input_X, mp_append_input_Y, mp_append_input_Z>;
static_assert(std::is_same<mp_append_expected, mp_append_actual>::value, "mp_append");

/* mp_map_from_list */
using mp_map_from_list_input    = mp_list<X1, X2, X3>;
using mp_map_from_list_expected = mp_list<mp_list<mp_size_t<0>, X1>, mp_list<mp_size_t<1>, X2>, mp_list<mp_size_t<2>, X3>>;
using mp_map_from_list_actual   = mp_map_from_list<mp_map_from_list_input>;
static_assert(std::is_same<mp_map_from_list_expected, mp_map_from_list_actual>::value, "mp_map_from_list");

/* mp_transform */
using mp_transform_input_0  = std::tuple<X1, X2, X3>;
using mp_transform_input_1  = std::tuple<Y1, Y2, Y3>;
using mp_transform_expected_0 = std::tuple<std::pair<X1, Y1>, std::pair<X2, Y2>, std::pair<X3, Y3>>;
using mp_transform_actual_0   = mp_transform<std::pair, mp_transform_input_0, mp_transform_input_1>;
static_assert(std::is_same<mp_transform_expected_0, mp_transform_actual_0>::value, "mp_transform");

using mp_transform_input_2    = mp_list<X1, X2, X3>;
using mp_transform_expected_1 = mp_list<X1*, X2*, X3*>;
using mp_transform_actual_1   = mp_transform<mp_add_pointer, mp_transform_input_2>;
static_assert(std::is_same<mp_transform_expected_1, mp_transform_actual_1>::value, "mp_transform");

/* unique id *************************************************************************************/

struct Counter1 { };
struct Counter2 { };

TEST(MetaProgrammingTest, unique_id)
{
    auto uid1_1 = nextUniqueId<Counter1>();
    auto uid1_2 = nextUniqueId<Counter1>();
    ASSERT_EQ(uid1_1, 0);
    ASSERT_EQ(uid1_2, 1);

    auto uid2_1 = getUniqueId<Counter2, int>();
    auto uid2_2 = getUniqueId<Counter2, int, float>();
    auto uid2_3 = getUniqueId<Counter2, float>();
    auto uid2_4 = getUniqueId<Counter2, int>();
    ASSERT_EQ(uid2_1, 0);
    ASSERT_EQ(uid2_2, 1);
    ASSERT_EQ(uid2_3, 2);
    ASSERT_EQ(uid2_4, 0);
}

TEST(MetaProgrammingTest, string)
{
    constexpr auto string1 = utl::mp_make_string("fuu bar");
    ASSERT_EQ ( string1.str(), std::string("fuu bar") );
}

TEST(MetaProgrammingTest, int_to_str)
{
    using t_int16 = utl::mp_const<int, 16>;
    constexpr auto decStr = utl::mp_int_to_string<t_int16, 10>();
    constexpr auto hexStr = utl::mp_int_to_string<t_int16, 16>();
    ASSERT_EQ ( decStr.str(), std::string("16") );
    ASSERT_EQ ( hexStr.str(), std::string("10") );
}