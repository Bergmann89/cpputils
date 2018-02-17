#include <gtest/gtest.h>
#include <cpputils/misc/stream.h>

namespace stream_helper_tests
{
    struct SimpleStruct
    {
        uint32_t    u32;
        uint8_t     u8;
    }
    __attribute__((__packed__));

    struct ManagedStruct
    {
        std::string s;

        void serialize(std::ostream& os) const
            { utl::stream_helper::write(os, s); }

        void deserialize(std::istream& is)
            { utl::stream_helper::read(is, s); }
    };
}

using namespace ::utl;
using namespace ::testing;
using namespace ::stream_helper_tests;

TEST(StreamHelperTests, write_scalar)
{
    std::ostringstream os;
    stream_helper::write(os, static_cast<uint32_t>(0x12345678));
    EXPECT_EQ(std::string("\x78\x56\x34\x12", 4), os.str());
}

TEST(StreamHelperTests, write_string)
{
    std::ostringstream os;
    stream_helper::write(os, std::string("test"));
    EXPECT_EQ(std::string("\x04\x00\x00\x00test", 8), os.str());
}

TEST(StreamHelperTests, write_simpleStruct)
{
    std::ostringstream os;
    stream_helper::write(os, SimpleStruct { 0x78563412, 5 });
    EXPECT_EQ(std::string("\x12\x34\x56\x78\x05", 5), os.str());
}

TEST(StreamHelperTests, write_managedStruct)
{
    std::ostringstream os;
    stream_helper::write(os, ManagedStruct { "test" });
    EXPECT_EQ(std::string("\x04\x00\x00\x00test", 8), os.str());
}

TEST(StreamHelperTests, read_scalar)
{
    std::istringstream is(std::string("\x78\x56\x34\x12", 4));
    auto ret = stream_helper::read<uint32_t>(is);
    EXPECT_EQ(0x12345678, ret);
}

TEST(StreamHelperTests, read_string)
{
    std::istringstream is(std::string("\x04\x00\x00\x00test", 8));
    auto ret = stream_helper::read<std::string>(is);
    EXPECT_EQ(std::string("test"), ret);
}

TEST(StreamHelperTests, read_simpleStruct)
{
    std::istringstream is(std::string("\x12\x34\x56\x78\x05", 5));
    auto ret = stream_helper::read<SimpleStruct>(is);
    EXPECT_EQ(0x78563412, ret.u32);
    EXPECT_EQ(5,          ret.u8);
}

TEST(StreamHelperTests, read_managedStruct)
{
    std::istringstream is(std::string("\x04\x00\x00\x00test", 8));
    auto ret = stream_helper::read<ManagedStruct>(is);
    EXPECT_EQ(std::string("test"), ret.s);
}