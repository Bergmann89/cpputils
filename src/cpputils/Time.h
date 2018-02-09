#pragma once

#include <chrono>
#include <utility>

namespace utl
{

    namespace __impl
    {
        template<class From, class To>
        struct op_convert_time;

        template<typename Rep, typename Period>
        struct op_convert_time<std::chrono::duration<Rep, Period>, ::timeval>
        {
            static ::timeval cast(const std::chrono::duration<Rep, Period>& d)
            {
                ::timeval tv;
                const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(d);
                tv.tv_sec  = sec.count();
                tv.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(d - sec).count();
                return std::move(tv);
            }

        };

        template<typename Rep, typename Period>
        struct op_convert_time<::timeval, std::chrono::duration<Rep, Period>>
        {
            static std::chrono::duration<Rep, Period> cast(const ::timeval& tv)
            {
                return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
                    std::chrono::seconds(tv.tv_sec) + std::chrono::microseconds(tv.tv_usec));
            }
        };

        template<typename Rep, typename Period>
        struct op_convert_time<std::chrono::duration<Rep, Period>, ::timespec>
        {
            static ::timespec cast(const std::chrono::duration<Rep, Period>& d)
            {
                ::timespec ts;
                const std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(d);
                ts.tv_sec  = sec.count();
                ts.tv_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(d - sec).count();
                return std::move(ts);
            }

        };

        template<typename Rep, typename Period>
        struct op_convert_time<::timespec, std::chrono::duration<Rep, Period>>
        {
            static std::chrono::duration<Rep, Period> cast(const ::timespec& ts)
            {
                return std::chrono::duration_cast<std::chrono::duration<Rep, Period>>(
                    std::chrono::seconds(ts.tv_sec) + std::chrono::nanoseconds(ts.tv_nsec));
            }
        };

    }

    template<typename T, typename Rep, typename Period>
    inline auto duration_cast(const std::chrono::duration<Rep, Period>& d)
        -> std::enable_if_t< std::is_same<T, ::timeval>::value, ::timeval>
        { return __impl::op_convert_time<std::chrono::duration<Rep, Period>, ::timeval>::cast(d); }

    template<typename T, typename Rep, typename Period>
    inline auto duration_cast(const std::chrono::duration<Rep, Period>& d)
        -> std::enable_if_t< std::is_same<T, ::timespec>::value, ::timespec>
        { return __impl::op_convert_time<std::chrono::duration<Rep, Period>, ::timespec>::cast(d); }

    template<typename Duration>
    Duration duration_cast(const ::timeval& tv)
        { return __impl::op_convert_time<::timeval, Duration>::cast(tv); }

    template<typename Duration>
    Duration duration_cast(const ::timespec& ts)
        { return __impl::op_convert_time<::timespec, Duration>::cast(ts); }

}