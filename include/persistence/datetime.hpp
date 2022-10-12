#pragma once
#include <chrono>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
namespace persistence
{
    namespace detail
    {
        inline std::time_t utc_to_posix(std::tm* cal) {
            return _mkgmtime(cal);
        }
    }
}
#else
namespace persistence
{
    namespace detail
    {
        inline std::time_t utc_to_posix(std::tm* cal) {
            return timegm(cal);
        }
    }
}
#endif

namespace persistence
{
    using timestamp = std::chrono::time_point<std::chrono::system_clock>;

    inline timestamp make_datetime(int year, int month, int day, int hour, int minute, int second)
    {
        std::tm tm{};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_hour = hour;
        tm.tm_min = minute;
        tm.tm_sec = second;
        std::time_t t = detail::utc_to_posix(&tm);
        return std::chrono::system_clock::from_time_t(t);
    }
}
