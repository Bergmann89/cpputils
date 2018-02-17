#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cpputils/logging/matcher.h>
#include <cpputils/logging/consumer.h>
#include <cpputils/logging/logger_impl.h>

using namespace ::testing;
using namespace ::utl::logging;

struct LoggingReset
{
    ~LoggingReset()
        { reset_logging(); }
};

struct consumer_mock : public consumer
{
    MOCK_METHOD1(log, void (data_ptr_s data));

    consumer_mock(const std::string& n) :
        consumer(n, true)
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

TEST(LoggingTests, matcher_all)
{
    LoggingReset loggingReset;
    consumer_stream c0("TestConsumer1", std::cout, false, false);
    consumer_stream c1("TestConsumer2", std::cout, false, false);

    auto& l0 = get_logger();
    auto& l1 = get_logger("TestLogger");

    matcher_all matcher;

    EXPECT_TRUE(matcher.match(c0));
    EXPECT_TRUE(matcher.match(c1));
    EXPECT_TRUE(matcher.match(l0));
    EXPECT_TRUE(matcher.match(l1));
}

TEST(LoggingTests, matcher_default)
{
    LoggingReset loggingReset;
    consumer_stream c0("TestConsumer1", std::cout, false, false);
    consumer_stream c1("TestConsumer2", std::cout, false, false);

    auto& l0 = get_logger();
    auto& l1 = get_logger("TestLogger");

    matcher_default matcher;

    EXPECT_FALSE(matcher.match(c0));
    EXPECT_FALSE(matcher.match(c1));
    EXPECT_TRUE (matcher.match(l0));
    EXPECT_FALSE(matcher.match(l1));
}

TEST(LoggingTests, matcher_regex)
{
    LoggingReset loggingReset;
    consumer_stream c0("TestConsumer1", std::cout, false, false);
    consumer_stream c1("TestConsumer2", std::cout, false, false);

    auto& l0 = get_logger();
    auto& l1 = get_logger("TestLogger");

    matcher_regex matcher0("TestConsumer1");
    matcher_regex matcher1("ASEF", true);

    EXPECT_TRUE (matcher0.match(c0));
    EXPECT_FALSE(matcher0.match(c1));
    EXPECT_FALSE(matcher1.match(l0));
    EXPECT_TRUE (matcher1.match(l1));
}

TEST(LoggingTests, log)
{
    LoggingReset loggingReset;
    StrictMock<consumer_mock> c0("consumer0");
    StrictMock<consumer_mock> c1("Consumer1");

    EXPECT_CALL(c0, log(MatchLogData(
        log_level::info,
        (void*)12,
        std::this_thread::get_id(),
        std::string("logger0"),
        std::string("test1 info"))));

    EXPECT_CALL(c0, log(MatchLogData(
        log_level::warn,
        (void*)13,
        std::this_thread::get_id(),
        std::string("logger0"),
        std::string("test1 warn"))));

    define_rule(matcher_ptr_u(new matcher_regex("logger0")), matcher_ptr_u(new matcher_regex("consumer0")), log_level::info, log_level::warn);

    auto& l0 = get_logger("logger0");
    auto& l1 = get_logger("logger1");

    log_message(l0, debug, (void*)11, "test1 ") << "debug";
    log_message(l0, info,  (void*)12, "test1 ") << "info";
    log_message(l0, warn,  (void*)13, "test1 ") << "warn";
    log_message(l0, error, (void*)14, "test1 ") << "error";

    log_message(l1, debug, (void*)21, "test2 ") << "debug";
    log_message(l1, info,  (void*)22, "test2 ") << "info";
    log_message(l1, warn,  (void*)23, "test2 ") << "warn";
    log_message(l1, error, (void*)24, "test2 ") << "error";
}

