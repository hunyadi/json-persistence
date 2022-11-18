#include <gtest/gtest.h>
#include "persistence/write_array.hpp"
#include "persistence/write_tuple.hpp"
#include "persistence/write_fundamental.hpp"
#include "persistence/write_object.hpp"
#include "persistence/write_string.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_array.hpp"
#include "persistence/serialize_tuple.hpp"
#include "persistence/serialize_fundamental.hpp"
#include "persistence/serialize_object.hpp"
#include "persistence/serialize_string.hpp"
#include "persistence/serialize.hpp"
#include "example_classes.hpp"
#include "test_serialize.hpp"

using namespace test;

TEST(Serialization, Array)
{
    EXPECT_TRUE(test_serialize(std::array<int, 0>(), "[]"));
    EXPECT_TRUE(test_serialize(std::array<int, 3>{1, 2, 3}, "[1,2,3]"));

    std::array<TestValue, 3> obj = { TestValue("a"), TestValue("b"), TestValue("c") };
    EXPECT_TRUE(test_serialize(obj, "[{\"value\":\"a\"},{\"value\":\"b\"},{\"value\":\"c\"}]"));

    std::array<std::string, 2> str = { "one", "two" };
    EXPECT_TRUE(test_serialize(str, "[\"one\",\"two\"]"));
}

TEST(Serialization, Pair)
{
    EXPECT_TRUE(test_serialize(std::make_pair(19, std::string("eighty-two")), "[19,\"eighty-two\"]"));
}

TEST(Serialization, Tuple)
{
    EXPECT_TRUE(test_serialize(std::tuple<>(), "[]"));
    EXPECT_TRUE(test_serialize(std::make_tuple(std::string("value")), "[\"value\"]"));
    EXPECT_TRUE(test_serialize(std::make_tuple(19, std::string("eighty-two")), "[19,\"eighty-two\"]"));
    EXPECT_TRUE(test_serialize(std::make_tuple(23, TestValue("string")), "[23,{\"value\":\"string\"}]"));
}
