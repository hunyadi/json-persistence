#include <gtest/gtest.h>
#include "persistence/parse_map.hpp"
#include "persistence/parse_set.hpp"
#include "persistence/parse_vector.hpp"
#include "persistence/parse_fundamental.hpp"
#include "persistence/parse_object.hpp"
#include "persistence/parse_string.hpp"
#include "persistence/parse.hpp"
#include "persistence/deserialize_map.hpp"
#include "persistence/deserialize_set.hpp"
#include "persistence/deserialize_vector.hpp"
#include "persistence/deserialize_fundamental.hpp"
#include "persistence/deserialize_object.hpp"
#include "persistence/deserialize_string.hpp"
#include "persistence/deserialize.hpp"
#include "example_classes.hpp"
#include "test_deserialize.hpp"

using namespace test;

TEST(Deserialization, Vector)
{
    EXPECT_TRUE(test_deserialize("[]", std::vector<int>()));
    EXPECT_TRUE(test_deserialize("[1, 2, 3]", std::vector<int> { 1, 2, 3 }));
    EXPECT_TRUE(test_deserialize("[1.5, 2.5, 3.5]", std::vector<float> { 1.5, 2.5, 3.5 }));
    EXPECT_TRUE(test_deserialize("[\"one\", \"two\"]", std::vector<std::string> { "one", "two" }));

    EXPECT_TRUE(test_deserialize(
        "[[1], [1, 2], [1, 2, 3]]",
        std::vector<std::vector<int>> { { 1 }, { 1,2 }, { 1,2,3 } }
    ));

    EXPECT_TRUE(test_deserialize(
        "[{\"value\":\"a\"}, {\"value\":\"b\"}, {}]",
        std::vector<TestDefault> { TestDefault("a"), TestDefault("b"), TestDefault() }
    ));
    EXPECT_TRUE(test_deserialize(
        "[{\"value\":\"a\"}, {\"value\":\"b\"}, {\"value\":\"default\"}]",
        std::vector<TestDefault> { TestDefault("a"), TestDefault("b"), TestDefault() }
    ));

    std::vector<TestValue> v;
    v.push_back(TestValue("a"));
    v.push_back(TestValue("b"));
    v.push_back(TestValue("c"));
    EXPECT_TRUE(test_deserialize("[{\"value\":\"a\"}, {\"value\":\"b\"}, {\"value\":\"c\"}]", v));

    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("[\"one\"]"));
    EXPECT_TRUE(test_no_deserialize<std::vector<std::string>>("[2,3]"));
    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("true"));
    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("23"));
    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("{}"));
}

TEST(Deserialization, Set)
{
    EXPECT_TRUE(test_deserialize("[]", std::set<int>()));
    EXPECT_TRUE(test_deserialize("[]", std::set<std::string>()));

    EXPECT_TRUE(test_deserialize("[1, 2, 3]", std::set<int> { 1, 2, 3 }));
    EXPECT_TRUE(test_deserialize("[1.5, 2.5, 3.5]", std::set<float> { 1.5, 2.5, 3.5 }));
    EXPECT_TRUE(test_deserialize("[\"one\", \"two\"]", std::set<std::string> { "one", "two" }));
    
    EXPECT_TRUE(test_deserialize(
        "[{\"value\":\"a\"}, {\"value\":\"b\"}, {\"value\":\"c\"}]",
        std::set<TestDefault> { TestDefault("c"), TestDefault("b"), TestDefault("a") }
    ));

    std::set<TestValue> s;
    s.insert(TestValue("a"));
    s.insert(TestValue("b"));
    s.insert(TestValue("c"));
    EXPECT_TRUE(test_deserialize("[{\"value\":\"a\"}, {\"value\":\"b\"}, {\"value\":\"c\"}]", s));

    EXPECT_TRUE(test_no_deserialize<std::set<int>>("[\"one\"]"));
    EXPECT_TRUE(test_no_deserialize<std::set<std::string>>("[2,3]"));
    EXPECT_TRUE(test_no_deserialize<std::set<int>>("true"));
    EXPECT_TRUE(test_no_deserialize<std::set<int>>("23"));
    EXPECT_TRUE(test_no_deserialize<std::set<int>>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<std::set<int>>("{}"));
}

TEST(Deserialization, Map)
{
    using map_type = std::map<std::string, int>;
    using string_map_type = std::map<std::string, std::string>;
    using vector_map_type = std::map<std::string, std::vector<int>>;

    EXPECT_TRUE(test_deserialize("{}", map_type()));
    EXPECT_TRUE(test_deserialize("{}", string_map_type()));

    EXPECT_TRUE(test_deserialize(
        "{\"key1\": 1, \"key2\": 2, \"key3\": 3}",
        map_type {
            { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
        }
    ));
    EXPECT_TRUE(test_deserialize(
        "{\"key1\": \"value1\", \"key2\": \"value2\"}",
        string_map_type {
            { "key1", "value1" }, { "key2", "value2" }
        }
    ));
    EXPECT_TRUE(test_deserialize(
        "{\"key1\": [1, 2], \"key2\": [4, 8, 16, 32]}",
        vector_map_type{
            { "key1", {1, 2} }, { "key2", {4, 8, 16, 32} }
        }
    ));

    std::map<std::string, TestValue> obj_map;
    obj_map.insert(std::make_pair("key1", TestValue("a")));
    obj_map.insert(std::make_pair("key2", TestValue("b")));
    obj_map.insert(std::make_pair("key3", TestValue("c")));
    EXPECT_TRUE(test_deserialize("{\"key1\":{\"value\":\"a\"},\"key2\":{\"value\":\"b\"},\"key3\":{\"value\":\"c\"}}", obj_map));

    EXPECT_TRUE(test_no_deserialize<map_type>("true"));
    EXPECT_TRUE(test_no_deserialize<map_type>("23"));
    EXPECT_TRUE(test_no_deserialize<map_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<map_type>("[]"));
    EXPECT_TRUE(test_no_deserialize<map_type>("[1]"));
}

TEST(Deserialization, UnorderedMap)
{
    using map_type = std::unordered_map<std::string, int>;
    using string_map_type = std::unordered_map<std::string, std::string>;

    EXPECT_TRUE(test_deserialize("{}", map_type()));
    EXPECT_TRUE(test_deserialize("{}", string_map_type()));

    EXPECT_TRUE(test_deserialize(
        "{\"key1\": 1, \"key2\": 2, \"key3\": 3}",
        map_type {
            { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
        }
    ));
    EXPECT_TRUE(test_deserialize(
        "{\"key1\": \"value1\", \"key2\": \"value2\"}",
        string_map_type {
            { "key1", "value1" }, { "key2", "value2" }
        }
    ));

    std::unordered_map<std::string, TestValue> obj_map;
    obj_map.insert(std::make_pair("key1", TestValue("a")));
    EXPECT_TRUE(test_deserialize("{\"key1\":{\"value\":\"a\"}}", obj_map));

    EXPECT_TRUE(test_no_deserialize<map_type>("true"));
    EXPECT_TRUE(test_no_deserialize<map_type>("23"));
    EXPECT_TRUE(test_no_deserialize<map_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<map_type>("[]"));
    EXPECT_TRUE(test_no_deserialize<map_type>("[1]"));
}
