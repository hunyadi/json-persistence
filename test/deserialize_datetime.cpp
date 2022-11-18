#include <gtest/gtest.h>
#include "persistence/parse_datetime.hpp"
#include "persistence/deserialize_datetime.hpp"
#if __cplusplus >= 202002L
#include "persistence/parse_date.hpp"
#include "persistence/deserialize_date.hpp"
#endif
#include "persistence/parse.hpp"
#include "persistence/deserialize.hpp"
#include "test_deserialize.hpp"

using namespace test;

TEST(Deserialization, DateTimeTypes)
{
    using persistence::make_datetime;
    using persistence::timestamp;

    EXPECT_TRUE(test_deserialize("\"1982-10-23T01:20:40Z\"", make_datetime(1982, 10, 23, 1, 20, 40)));
    EXPECT_TRUE(test_deserialize("\"2022-02-01T23:02:01Z\"", make_datetime(2022, 2, 1, 23, 2, 1)));
    EXPECT_TRUE(test_deserialize("\"2000-02-29T00:00:00Z\"", make_datetime(2000, 2, 29, 0, 0, 0)));
    EXPECT_TRUE(test_deserialize("\"2000-03-31T00:00:00Z\"", make_datetime(2000, 3, 31, 0, 0, 0)));

    // space instead of 'T'
    EXPECT_TRUE(test_deserialize("\"2022-02-01 23:02:01Z\"", make_datetime(2022, 2, 1, 23, 2, 1)));

    // two-digit year
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"22-02-01T23:02:01Z\""));

    // missing or wrong time zone
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:02:01\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:02:01A\""));

    // invalid year, month or day
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"YYYY-02-01T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-MM-01T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-00-01T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-99-01T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-DDT23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-00T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-30T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-99T23:02:01Z\""));

    // invalid hour, minute or second
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01THH:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T99:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:MM:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:99:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:02:SSZ\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:02:99Z\""));

    // invalid type
    EXPECT_TRUE(test_no_deserialize<timestamp>("true"));
    EXPECT_TRUE(test_no_deserialize<timestamp>("23"));
    EXPECT_TRUE(test_no_deserialize<timestamp>("[]"));
    EXPECT_TRUE(test_no_deserialize<timestamp>("{}"));

#if __cplusplus >= 202002L
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    EXPECT_TRUE(test_deserialize("\"1982-10-23\"", year_month_day(1982y, October, 23d)));
    EXPECT_TRUE(test_deserialize("\"2022-02-01\"", year_month_day(2022y, February, 1d)));
    EXPECT_TRUE(test_deserialize("\"2000-02-29\"", year_month_day(2000y, February, 29d)));
    EXPECT_TRUE(test_deserialize("\"2000-03-31\"", year_month_day(2000y, March, 31d)));

    // two-digit year
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"22-02-01\""));

    // invalid year, month or day
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"YYYY-02-01\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-MM-01\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-00-01\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-99-01\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-02-DD\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-02-00\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-02-30\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-02-99\""));

    // invalid type
    EXPECT_TRUE(test_no_deserialize<year_month_day>("true"));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("23"));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("[]"));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("{}"));
#endif
}
