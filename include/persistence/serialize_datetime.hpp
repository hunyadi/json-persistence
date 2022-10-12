#pragma once
#include "detail/engine.hpp"
#include "serialize_allocate.hpp"
#include "serialize_base.hpp"
#include "datetime.hpp"
#include <charconv>
#include <chrono>
#include <string>

namespace persistence
{
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

    template<>
    struct JsonSerializer<timestamp> : JsonAllocatingSerializer
    {
        using JsonAllocatingSerializer::JsonAllocatingSerializer;

        bool operator()(timestamp value, rapidjson::Value& json) const
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
            char text[] = "0000-00-00T00:00:00Z";
            using detail::to_aligned_digits;
            to_aligned_digits<4>(text, year);
            to_aligned_digits<2>(text + 5, month);
            to_aligned_digits<2>(text + 8, day);
            to_aligned_digits<2>(text + 11, hour);
            to_aligned_digits<2>(text + 14, minute);
            to_aligned_digits<2>(text + 17, second);

            // emit JSON value
            json.SetString(text, 20, allocator);
            return true;
        }
    };
}
