#include <gtest/gtest.h>
#include "../src/cpputils/StringHelper.h"
#include "../src/cpputils/HandleManager.h"

using namespace utl;

using HandleManagerInt = utl::HandleManager<int>;

TEST(HandleManagerTest, fromString_toString)
{
    Handle handle;
    EXPECT_FALSE(tryFromString("11-22-3344-5566778", handle));
    EXPECT_TRUE (tryFromString("11-22-3344-55667788", handle));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), toString(handle));
    EXPECT_TRUE (tryFromString("1122-3344-55667788", handle));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), toString(handle));
    EXPECT_TRUE (tryFromString("11-223344-55667788", handle));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), toString(handle));
    EXPECT_TRUE (tryFromString("11-22-334455667788", handle));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), toString(handle));
    EXPECT_TRUE (tryFromString("1122334455667788", handle));
    EXPECT_EQ   (std::string("11-22-3344-55667788"), toString(handle));
}

TEST(HandleManagerTest, insert)
{
    HandleManagerInt manager;
    auto handle = manager.insert(0, 0, 123);
    EXPECT_TRUE(static_cast<bool>(handle));
}

TEST(HandleManagerTest, remove)
{
    HandleManagerInt manager;
    auto handle = manager.insert(0, 0, 123);
    EXPECT_FALSE(manager.remove(156161));
    EXPECT_FALSE(manager.remove(1136));
    EXPECT_FALSE(manager.remove(627624));
    EXPECT_FALSE(manager.remove(0));
    EXPECT_TRUE (manager.remove(handle));
}

TEST(HandleManagerTest, clear)
{
    HandleManagerInt manager;
    auto handle1 = manager.insert(0, 0, 123);
    manager.clear();
    auto handle2 = manager.insert(0, 0, 555);
    EXPECT_EQ(handle1, handle2);
}

TEST(HandleManagerTest, isValid)
{
    HandleManagerInt manager;
    EXPECT_FALSE(manager.isValid(0));
    EXPECT_FALSE(manager.isValid(123));
    EXPECT_FALSE(manager.isValid(51627));
    EXPECT_FALSE(manager.isValid(1513));
    EXPECT_FALSE(manager.isValid(16621));

    auto handle = manager.insert(0, 0, 123);
    EXPECT_TRUE(manager.isValid(handle));
}

TEST(HandleManagerTest, tryGet)
{
    int val;
    HandleManagerInt manager;
    EXPECT_FALSE(manager.tryGet(123, val));
    EXPECT_FALSE(manager.tryGet(5132, val));
    EXPECT_FALSE(manager.tryGet(6216, val));
    EXPECT_FALSE(manager.tryGet(15616724, val));
    EXPECT_FALSE(manager.tryGet(12353, val));

    auto handle = manager.insert(1, 2, 555);
    EXPECT_TRUE(manager.tryGet(handle, val));
    EXPECT_EQ  (555, val);
}

TEST(HandleManagerTest, get)
{
    HandleManagerInt manager;
    EXPECT_ANY_THROW(manager.get(123));
    EXPECT_ANY_THROW(manager.get(5132));
    EXPECT_ANY_THROW(manager.get(6216));
    EXPECT_ANY_THROW(manager.get(15616724));
    EXPECT_ANY_THROW(manager.get(12353));

    auto handle = manager.insert(1, 2, 555);
    EXPECT_EQ(555, manager.get(handle));
}

TEST(HandleManagerTest, update)
{
    HandleManagerInt manager;
    EXPECT_FALSE(manager.update(123, 555));
    EXPECT_FALSE(manager.update(5132, 555));
    EXPECT_FALSE(manager.update(6216, 555));
    EXPECT_FALSE(manager.update(15616724, 555));
    EXPECT_FALSE(manager.update(12353, 555));

    auto handle = manager.insert(1, 2, 555);
    EXPECT_TRUE(manager.update(handle, 554));
    EXPECT_EQ  (554, manager.get(handle));
}

TEST(HandleManagerTest, set)
{
    HandleManagerInt manager;
    auto handle = fromString<utl::Handle>("00-00-0001-00000000");
    EXPECT_TRUE (manager.set(handle, 555));
    EXPECT_EQ   (555, manager.get(handle));

    handle = fromString<utl::Handle>("00-00-0001-00000000");
    EXPECT_TRUE (manager.set(handle, 554));
    EXPECT_EQ   (554, manager.get(handle));

    EXPECT_FALSE(manager.set(fromString<utl::Handle>("00-00-0002-00000000"), 553));
    EXPECT_EQ   (554, manager.get(handle));
}