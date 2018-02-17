#include <gtest/gtest.h>
#include <cpputils/mp/util/to.h>

using namespace ::utl::mp;

static_assert(is_convertible<int32_t, int8_t>::value, "is_convertible");
static_assert(is_convertible<uint32_t, int8_t>::value, "is_convertible");
static_assert(!is_convertible<std::string, int8_t>::value, "is_convertible");

static_assert(is_embedded<double,   long double>::value, "is_embdedded");
static_assert(is_embedded<float,    long double>::value, "is_embdedded");
static_assert(is_embedded<float,    double     >::value, "is_embdedded");

static_assert(is_embedded<signed long,  signed long long>::value, "is_embedded");
static_assert(is_embedded<signed int,   signed long long>::value, "is_embedded");
static_assert(is_embedded<signed short, signed long long>::value, "is_embedded");
static_assert(is_embedded<signed char,  signed long long>::value, "is_embedded");
static_assert(is_embedded<signed int,   signed long     >::value, "is_embedded");
static_assert(is_embedded<signed short, signed long     >::value, "is_embedded");
static_assert(is_embedded<signed char,  signed long     >::value, "is_embedded");
static_assert(is_embedded<signed short, signed int      >::value, "is_embedded");
static_assert(is_embedded<signed char,  signed int      >::value, "is_embedded");
static_assert(is_embedded<signed char,  signed short    >::value, "is_embedded");

static_assert(is_embedded<unsigned long,  unsigned long long>::value, "is_embedded");
static_assert(is_embedded<unsigned int,   unsigned long long>::value, "is_embedded");
static_assert(is_embedded<unsigned short, unsigned long long>::value, "is_embedded");
static_assert(is_embedded<unsigned char,  unsigned long long>::value, "is_embedded");
static_assert(is_embedded<unsigned int,   unsigned long     >::value, "is_embedded");
static_assert(is_embedded<unsigned short, unsigned long     >::value, "is_embedded");
static_assert(is_embedded<unsigned char,  unsigned long     >::value, "is_embedded");
static_assert(is_embedded<unsigned short, unsigned int      >::value, "is_embedded");
static_assert(is_embedded<unsigned char,  unsigned int      >::value, "is_embedded");
static_assert(is_embedded<unsigned char,  unsigned short    >::value, "is_embedded");

TEST(test_mp_util_to, make)
{
    EXPECT_EQ(to<int>(static_cast<uint8_t>(8)), 8);
}