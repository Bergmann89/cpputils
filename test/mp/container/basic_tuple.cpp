#include <cpputils/mp/core/const.h>
#include <cpputils/mp/container/basic_tuple.h>

using namespace ::utl::mp;

namespace test_mp_container_basic_tuple
{
    struct op_unpack
    {
        constexpr auto operator()(const c_bool_t<false>&, const c_size_t<5>&)
            { return true; }
    };

    template<typename T>
    struct Wrapper { };

    struct op_transform
    {
        template<typename T>
        constexpr auto operator()(T&& t)
            { return Wrapper<clean_type<T>> { }; }
    };

    constexpr auto my_tuple = make_basic_tuple(c_bool_t<false> { }, c_size_t<5> { });
    constexpr c_size_t<0> index0;
    constexpr c_size_t<1> index1;

    static_assert(unpack(my_tuple, op_unpack { }), "");
    static_assert(my_tuple.unpack(op_unpack { }), "");

    static_assert(decltype(at(my_tuple, index0))::value == false, "");
    static_assert(decltype(at(my_tuple, index1))::value == 5, "");
    static_assert(decltype(my_tuple.at(index0))::value == false, "");
    static_assert(decltype(my_tuple.at(index1))::value == 5, "");

    constexpr auto transformed = my_tuple.transform(op_transform { });
    using transformed_type     = clean_type<decltype(transformed)>;
    using transformed_expected = basic_tuple<Wrapper<c_bool_t<false>>, Wrapper<c_size_t<5>>>;

    static_assert(is_same<transformed_type, transformed_expected>::value, "");
}