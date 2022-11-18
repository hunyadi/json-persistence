#include <gtest/gtest.h>
#include "persistence/write_datetime.hpp"
#include "persistence/serialize_datetime.hpp"
#if __cplusplus >= 202002L
#include "persistence/write_date.hpp"
#include "persistence/serialize_date.hpp"
#endif
#include "persistence/write.hpp"
#include "persistence/serialize.hpp"
#include "test_serialize.hpp"

using namespace test;

TEST(Serialization, DateTimeTypes)
{
    using persistence::make_datetime;

    EXPECT_TRUE(test_serialize(make_datetime(1982, 10, 23, 1, 20, 40), "\"1982-10-23T01:20:40Z\""));
    EXPECT_TRUE(test_serialize(make_datetime(2022, 2, 1, 23, 2, 1), "\"2022-02-01T23:02:01Z\""));

#if __cplusplus >= 202002L
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    EXPECT_TRUE(test_serialize(year_month_day(1982y, October, 23d), "\"1982-10-23\""));
    EXPECT_TRUE(test_serialize(year_month_day(2022y, February, 1d), "\"2022-02-01\""));
#endif
}
