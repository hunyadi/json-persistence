#pragma once
#include "detail/unlikely.hpp"
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
    inline timestamp make_datetime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second)
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

    inline constexpr bool is_leap_year(unsigned year)
    {
        return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    }

    inline constexpr bool is_valid_date(unsigned year, unsigned month, unsigned day)
    {
        if (day < 1) {
            return false;
        }
        switch (month) {
            case 1:  // January
            case 3:  // March
            case 5:  // May
            case 7:  // July
            case 8:  // August
            case 10:  // October
            case 12:  // December
                return day <= 31;

            case 4:  // April
            case 6:  // June
            case 9:  // September
            case 11:  // November
                return day <= 30;

            case 2:  // February
                if (is_leap_year(year)) {
                    return day <= 29;
                } else {
                    return day <= 28;
                }

            default:
                return false;
        }
    }

    namespace detail
    {
        inline bool from_digits(const char* first, const char* last, unsigned& value)
        {
            auto result = std::from_chars(first, last, value);
            return result.ec == std::errc() && result.ptr == last;
        }
    }

    /**
     * Parses a chrono time point from an ISO 8601 date string.
     */
    inline bool parse_date(const char* text, std::size_t len, timestamp& value)
    {
        PERSISTENCE_IF_UNLIKELY(len != 10) {
            return false;
        }

        PERSISTENCE_IF_UNLIKELY(text[4] != '-' || text[7] != '-') {
            return false;
        }

        using detail::from_digits;
        unsigned year, month, day;
        PERSISTENCE_IF_UNLIKELY(
            !from_digits(text, text + 4, year) ||
            !from_digits(text + 5, text + 7, month) ||
            !from_digits(text + 8, text + 10, day)
        ) {
            return false;
        }

        PERSISTENCE_IF_UNLIKELY(!is_valid_date(year, month, day)) {
            return false;
        }

        value = make_datetime(year, month, day, 0, 0, 0);
        return true;
    }

    /**
     * Parses a chrono time point from an ISO 8601 date-time string.
     */
    inline bool parse_datetime(const char* text, std::size_t len, timestamp& value)
    {
        PERSISTENCE_IF_UNLIKELY(len != 20) {
            return false;
        }

        PERSISTENCE_IF_UNLIKELY(
            text[4] != '-' || text[7] != '-' ||
            (text[10] != 'T' && text[10] != ' ') ||
            text[13] != ':' || text[16] != ':' || text[19] != 'Z'
        ) {
            return false;
        }

        using detail::from_digits;
        unsigned year, month, day, hour, minute, second;
        PERSISTENCE_IF_UNLIKELY(
            !from_digits(text, text + 4, year) ||
            !from_digits(text + 5, text + 7, month) ||
            !from_digits(text + 8, text + 10, day) ||
            !from_digits(text + 11, text + 13, hour) ||
            !from_digits(text + 14, text + 16, minute) ||
            !from_digits(text + 17, text + 19, second)
        ) {
            return false;
        }

        PERSISTENCE_IF_UNLIKELY(!is_valid_date(year, month, day)) {
            return false;
        }
        PERSISTENCE_IF_UNLIKELY(hour >= 24 || minute >= 60 || second >= 60) {
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
    struct iso_date
    {
        char text[11] = "0000-00-00";

        char* year() { return text; }
        char* month() { return text + 5; }
        char* day() { return text + 8; }

        constexpr static std::size_t length() { return 10; }
    };

    /**
     * Represents an ISO 8601 date-time string.
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
    inline iso_date to_iso_date(timestamp value)
    {
        // get number of seconds since the UNIX epoch: Thu, January 1, 1970, 00:00:00 UTC
        std::time_t posix_time = std::chrono::system_clock::to_time_t(value);

        // convert to calendar time in timezone UTC
        auto calendar_time = gmtime(&posix_time);
        int year = calendar_time->tm_year + 1900;
        int month = calendar_time->tm_mon + 1;
        int day = calendar_time->tm_mday;

        // format to seconds resolution, e.g. 1982-10-23T01:20:40Z
        iso_date dt;
        using detail::to_aligned_digits;
        to_aligned_digits<4>(dt.year(), year);
        to_aligned_digits<2>(dt.month(), month);
        to_aligned_digits<2>(dt.day(), day);

        return dt;
    }

    /**
     * Converts a chrono time point to an ISO 8601 date-time string.
     */
    inline iso_datetime to_iso_datetime(timestamp value)
    {
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
