#include <gtest/gtest.h>
#include "persistence/parse_array.hpp"
#include "persistence/parse_fundamental.hpp"
#include "persistence/parse_object.hpp"
#include "persistence/parse_string.hpp"
#include "persistence/parse.hpp"
#include "persistence/deserialize_array.hpp"
#include "persistence/deserialize_tuple.hpp"
#include "persistence/deserialize_fundamental.hpp"
#include "persistence/deserialize_object.hpp"
#include "persistence/deserialize_string.hpp"
#include "persistence/deserialize.hpp"
#include "example_classes.hpp"
#include "test_deserialize.hpp"

using namespace test;

TEST(Deserialization, Array)
{
    EXPECT_TRUE(test_deserialize("[]", std::array<int, 0> {}));
    EXPECT_TRUE(test_deserialize("[1, 2, 3]", std::array<int, 3> { 1, 2, 3 }));
    EXPECT_TRUE(test_deserialize("[\"one\", \"two\"]", std::array<std::string, 2> { "one", "two" }));
    EXPECT_TRUE(test_deserialize(
        "[{\"value\":\"a\"},{\"value\":\"b\"},{\"value\":\"c\"}]",
        std::array<TestValue, 3> { TestValue("a"), TestValue("b"), TestValue("c") }
    ));

    using int_array_type = std::array<int, 2>;
    using string_array_type = std::array<std::string, 2>;
    EXPECT_TRUE(test_no_deserialize<int_array_type>("[]"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("[1]"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("[1,2,3]"));
    EXPECT_TRUE(test_no_deserialize<string_array_type>("[1,2]"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("[\"one\",\"two\"]"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("true"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("23"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("{}"));
}

TEST(Deserialization, Pair)
{
    EXPECT_TRUE(test_deserialize("[19, \"eighty-two\"]", std::make_pair(19, std::string("eighty-two"))));

    using pair_type = std::pair<int, std::string>;
    EXPECT_TRUE(test_no_deserialize<pair_type>("[1]"));
    EXPECT_TRUE(test_no_deserialize<pair_type>("[1,\"string1\",23,\"string2\"]"));
    EXPECT_TRUE(test_no_deserialize<pair_type>("true"));
    EXPECT_TRUE(test_no_deserialize<pair_type>("23"));
    EXPECT_TRUE(test_no_deserialize<pair_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<pair_type>("{}"));
}

TEST(Deserialization, Tuple)
{
    EXPECT_TRUE(test_deserialize("[]", std::tuple<>()));
    EXPECT_TRUE(test_deserialize("[\"value\"]", std::make_tuple(std::string("value"))));
    EXPECT_TRUE(test_deserialize("[19, \"eighty-two\"]", std::make_tuple(19, std::string("eighty-two"))));
    EXPECT_TRUE(test_deserialize("[23, {\"value\": \"string\"}]", std::make_tuple(23, TestValue("string"))));

    using tuple_type = std::tuple<int, std::string>;
    EXPECT_TRUE(test_no_deserialize<tuple_type>("[1]"));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("[1,\"string1\",23,\"string2\"]"));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("true"));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("23"));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("{}"));
}
