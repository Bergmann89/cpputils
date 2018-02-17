#include <gtest/gtest.h>
#include <cpputils/misc/string.h>
#include <cpputils/container/handle_manager.h>

using namespace utl;

using handle_manager_int = utl::handle_manager<int>;

TEST(handle_manager_tests, from_string_to_string)
{
    handle h;
    EXPECT_FALSE(try_from_string("11-22-3344-5566778", h));
    EXPECT_TRUE (try_from_string("11-22-3344-55667788", h));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), to_string(h));
    EXPECT_TRUE (try_from_string("1122-3344-55667788", h));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), to_string(h));
    EXPECT_TRUE (try_from_string("11-223344-55667788", h));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), to_string(h));
    EXPECT_TRUE (try_from_string("11-22-334455667788", h));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), to_string(h));
    EXPECT_TRUE (try_from_string("1122334455667788", h));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), to_string(h));
}

TEST(handle_manager_tests, insert)
{
    handle_manager_int manager;
    auto handle = manager.insert(0, 0, 123);
    EXPECT_TRUE(static_cast<bool>(handle));
}

TEST(handle_manager_tests, remove)
{
    handle_manager_int manager;
    auto handle = manager.insert(0, 0, 123);
    EXPECT_FALSE(manager.remove(156161));
    EXPECT_FALSE(manager.remove(1136));
    EXPECT_FALSE(manager.remove(627624));
    EXPECT_FALSE(manager.remove(0));
    EXPECT_TRUE (manager.remove(handle));
}

TEST(handle_manager_tests, clear)
{
    handle_manager_int manager;
    auto handle1 = manager.insert(0, 0, 123);
    manager.clear();
    auto handle2 = manager.insert(0, 0, 555);
    EXPECT_EQ(handle1, handle2);
}

TEST(handle_manager_tests, is_valid)
{
    handle_manager_int manager;
    EXPECT_FALSE(manager.is_valid(0));
    EXPECT_FALSE(manager.is_valid(123));
    EXPECT_FALSE(manager.is_valid(51627));
    EXPECT_FALSE(manager.is_valid(1513));
    EXPECT_FALSE(manager.is_valid(16621));

    auto handle = manager.insert(0, 0, 123);
    EXPECT_TRUE(manager.is_valid(handle));
}

TEST(handle_manager_tests, try_get)
{
    int val;
    handle_manager_int manager;
    EXPECT_FALSE(manager.try_get(123, val));
    EXPECT_FALSE(manager.try_get(5132, val));
    EXPECT_FALSE(manager.try_get(6216, val));
    EXPECT_FALSE(manager.try_get(15616724, val));
    EXPECT_FALSE(manager.try_get(12353, val));

    auto handle = manager.insert(1, 2, 555);
    EXPECT_TRUE(manager.try_get(handle, val));
    EXPECT_EQ  (555, val);
}

TEST(handle_manager_tests, get)
{
    handle_manager_int manager;
    EXPECT_ANY_THROW(manager.get(123));
    EXPECT_ANY_THROW(manager.get(5132));
    EXPECT_ANY_THROW(manager.get(6216));
    EXPECT_ANY_THROW(manager.get(15616724));
    EXPECT_ANY_THROW(manager.get(12353));

    auto handle = manager.insert(1, 2, 555);
    EXPECT_EQ(555, manager.get(handle));
}

TEST(handle_manager_tests, update)
{
    handle_manager_int manager;
    EXPECT_FALSE(manager.update(123, 555));
    EXPECT_FALSE(manager.update(5132, 555));
    EXPECT_FALSE(manager.update(6216, 555));
    EXPECT_FALSE(manager.update(15616724, 555));
    EXPECT_FALSE(manager.update(12353, 555));

    auto handle = manager.insert(1, 2, 555);
    EXPECT_TRUE(manager.update(handle, 554));
    EXPECT_EQ  (554, manager.get(handle));
}

TEST(handle_manager_tests, set)
{
    handle_manager_int manager;
    auto handle = from_string<utl::handle>("00-00-0001-00000000");
    EXPECT_TRUE (manager.set(handle, 555));
    EXPECT_EQ   (555, manager.get(handle));

    handle = from_string<utl::handle>("00-00-0001-00000000");
    EXPECT_TRUE (manager.set(handle, 554));
    EXPECT_EQ   (554, manager.get(handle));

    EXPECT_FALSE(manager.set(from_string<utl::handle>("00-00-0002-00000000"), 553));
    EXPECT_EQ   (554, manager.get(handle));
}