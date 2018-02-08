#include <set>
#include <list>
#include <regex>
#include <mutex>
#include <thread>
#include <memory>
#include <chrono>
#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace logging_tests
{
    #include "../src/LoggerImpl.cpp"
}

using namespace ::testing;
using namespace ::logging_tests;
using namespace ::logging_tests::logging;

struct LoggingReset
{
    ~LoggingReset()
        { ::logging_tests::logging::resetLogging(); }
};

struct ConsumerMock : public Consumer
{
    MOCK_METHOD1(log, void (DataPtrS data));

    ConsumerMock(const std::string& n) :
        Consumer(n, true)
        { }
};

MATCHER_P5(MatchLogData, level, sender, thread, name, message, "")
{
    if (!arg)
        return false;
    auto& d = *arg;
    return d.level   == level
        && d.sender  == sender
        && d.thread  == thread
        && d.name    == name
        && d.message == message;
}

TEST(LoggingTests, AllMatcher)
{
    LoggingReset loggingReset;
    StreamConsumer c0("TestConsumer1", std::cout, false, false);
    StreamConsumer c1("TestConsumer2", std::cout, false, false);

    auto& l0 = getLogger();
    auto& l1 = getLogger("TestLogger");

    AllMatcher matcher;

    EXPECT_TRUE(matcher.match(c0));
    EXPECT_TRUE(matcher.match(c1));
    EXPECT_TRUE(matcher.match(l0));
    EXPECT_TRUE(matcher.match(l1));
}

TEST(LoggingTests, DefaultLoggerMatcher)
{
    LoggingReset loggingReset;
    StreamConsumer c0("TestConsumer1", std::cout, false, false);
    StreamConsumer c1("TestConsumer2", std::cout, false, false);

    auto& l0 = getLogger();
    auto& l1 = getLogger("TestLogger");

    DefaultLoggerMatcher matcher;

    EXPECT_FALSE(matcher.match(c0));
    EXPECT_FALSE(matcher.match(c1));
    EXPECT_TRUE (matcher.match(l0));
    EXPECT_FALSE(matcher.match(l1));
}

TEST(LoggingTests, RegexMatcher)
{
    LoggingReset loggingReset;
    StreamConsumer c0("TestConsumer1", std::cout, false, false);
    StreamConsumer c1("TestConsumer2", std::cout, false, false);

    auto& l0 = getLogger();
    auto& l1 = getLogger("TestLogger");

    RegexMatcher matcher0("TestConsumer1");
    RegexMatcher matcher1("ASEF", true);

    EXPECT_TRUE (matcher0.match(c0));
    EXPECT_FALSE(matcher0.match(c1));
    EXPECT_FALSE(matcher1.match(l0));
    EXPECT_TRUE (matcher1.match(l1));
}

TEST(LoggingTests, log)
{
    LoggingReset loggingReset;
    StrictMock<ConsumerMock> c0("consumer0");
    StrictMock<ConsumerMock> c1("Consumer1");

    EXPECT_CALL(c0, log(MatchLogData(
        Level::Info,
        (void*)12,
        std::this_thread::get_id(),
        std::string("logger0"),
        std::string("test1 info"))));

    EXPECT_CALL(c0, log(MatchLogData(
        Level::Warn,
        (void*)13,
        std::this_thread::get_id(),
        std::string("logger0"),
        std::string("test1 warn"))));

    defineRule(MatcherPtrU(new RegexMatcher("logger0")), MatcherPtrU(new RegexMatcher("consumer0")), Level::Info, Level::Warn);

    auto& l0 = getLogger("logger0");
    auto& l1 = getLogger("logger1");

    logMessage(l0, Debug, (void*)11, "test1 ") << "debug";
    logMessage(l0, Info,  (void*)12, "test1 ") << "info";
    logMessage(l0, Warn,  (void*)13, "test1 ") << "warn";
    logMessage(l0, Error, (void*)14, "test1 ") << "error";

    logMessage(l1, Debug, (void*)21, "test2 ") << "debug";
    logMessage(l1, Info,  (void*)22, "test2 ") << "info";
    logMessage(l1, Warn,  (void*)23, "test2 ") << "warn";
    logMessage(l1, Error, (void*)24, "test2 ") << "error";
}

