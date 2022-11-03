#pragma once
#include <charconv>
#include <chrono>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
namespace persistence
{
    namespace detail
    {
        /**
         * Converts a Gregorian calendar UTC timestamp into a POSIX timestamp (seconds since epoch).
         */
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
        /**
         * Converts a Gregorian calendar UTC timestamp into a POSIX timestamp (seconds since epoch).
         */
        inline std::time_t utc_to_posix(std::tm* cal) {
            return timegm(cal);
        }
    }
}
#endif

namespace persistence
{
    using timestamp = std::chrono::time_point<std::chrono::system_clock>;

    /**
     * Creates a chrono time point from a Gregorian calendar date and UTC time.
     */
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

    namespace detail
    {
        inline bool from_digits(const char* first, const char* last, int& value)
        {
            auto result = std::from_chars(first, last, value);
            return result.ec == std::errc() && result.ptr == last;
        }
    }

    /**
     * Parses a chrono time point from an ISO 8601 date string.
     */
    inline bool parse_datetime(const char* text, std::size_t len, timestamp& value)
    {
        if (len != 20) {
            return false;
        }

        if (text[4] != '-' ||
            text[7] != '-' ||
            (text[10] != 'T' && text[10] != ' ') ||
            text[13] != ':' ||
            text[16] != ':' ||
            text[19] != 'Z'
        ) {
            return false;
        }

        using detail::from_digits;
        int year, month, day, hour, minute, second;
        if (!from_digits(text, text + 4, year) ||
            !from_digits(text + 5, text + 7, month) ||
            !from_digits(text + 8, text + 10, day) ||
            !from_digits(text + 11, text + 13, hour) ||
            !from_digits(text + 14, text + 16, minute) ||
            !from_digits(text + 17, text + 19, second)
        ) {
            return false;
        }

        value = make_datetime(year, month, day, hour, minute, second);
        return true;
    }

    namespace detail
    {
        template<std::size_t N>
        struct AlignedDigitWriter {
            std::to_chars_result operator()(char* ptr, int value) const;
        };

        template<>
        struct AlignedDigitWriter<4> {
            std::to_chars_result operator()(char* ptr, int value) const {
                char* last = ptr + 4;
                if (value < 10) {
                    ptr += 3;
                } else if (value < 100) {
                    ptr += 2;
                } else if (value < 1000) {
                    ptr += 1;
                }
                return std::to_chars(ptr, last, value);
            }
        };

        template<>
        struct AlignedDigitWriter<2> {
            std::to_chars_result operator()(char* ptr, int value) const {
                char* last = ptr + 2;
                if (value < 10) {
                    ptr += 1;
                }
                return std::to_chars(ptr, last, value);
            }
        };

        template<std::size_t N>
        std::to_chars_result to_aligned_digits(char* ptr, int value) {
            AlignedDigitWriter<N> writer;
            return writer(ptr, value);
        }
    }

    /**
     * Represents an ISO 8601 date string.
     */
    struct iso_datetime
    {
        char text[21] = "0000-00-00T00:00:00Z";

        char* year() { return text; }
        char* month() { return text + 5; }
        char* day() { return text + 8; }
        char* hour() { return text + 11; }
        char* minute() { return text + 14; }
        char* second() { return text + 17; }

        constexpr static std::size_t length() { return 20; }
    };

    /**
     * Converts a chrono time point to an ISO 8601 date string.
     */
    inline iso_datetime to_iso_datetime(timestamp value) {
        // get number of seconds since the UNIX epoch: Thu, January 1, 1970, 00:00:00 UTC
        std::time_t posix_time = std::chrono::system_clock::to_time_t(value);

        // convert to calendar time in timezone UTC
        auto calendar_time = gmtime(&posix_time);
        int year = calendar_time->tm_year + 1900;
        int month = calendar_time->tm_mon + 1;
        int day = calendar_time->tm_mday;
        int hour = calendar_time->tm_hour;
        int minute = calendar_time->tm_min;
        int second = calendar_time->tm_sec;

        // format to seconds resolution, e.g. 1982-10-23T01:20:40Z
        iso_datetime dt;
        using detail::to_aligned_digits;
        to_aligned_digits<4>(dt.year(), year);
        to_aligned_digits<2>(dt.month(), month);
        to_aligned_digits<2>(dt.day(), day);
        to_aligned_digits<2>(dt.hour(), hour);
        to_aligned_digits<2>(dt.minute(), minute);
        to_aligned_digits<2>(dt.second(), second);

        return dt;
    }
}
