#include <gtest/gtest.h>
#include "persistence/write_map.hpp"
#include "persistence/write_set.hpp"
#include "persistence/write_vector.hpp"
#include "persistence/write_fundamental.hpp"
#include "persistence/write_object.hpp"
#include "persistence/write_string.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_map.hpp"
#include "persistence/serialize_set.hpp"
#include "persistence/serialize_vector.hpp"
#include "persistence/serialize_fundamental.hpp"
#include "persistence/serialize_object.hpp"
#include "persistence/serialize_string.hpp"
#include "persistence/serialize.hpp"
#include "example_classes.hpp"
#include "test_serialize.hpp"

using namespace test;

TEST(Serialization, Vector)
{
    EXPECT_TRUE(test_serialize(std::vector<int>(), "[]"));
    EXPECT_TRUE(test_serialize(std::vector<int>{1, 2, 3}, "[1,2,3]"));
    EXPECT_TRUE(test_serialize(std::vector<float>{1.5, 2.5, 3.5}, "[1.5,2.5,3.5]"));
    EXPECT_TRUE(test_serialize(std::vector<std::vector<int>>{ { 1 }, { 1,2 }, { 1,2,3 } }, "[[1],[1,2],[1,2,3]]"));

    EXPECT_TRUE(test_serialize(
        std::vector<TestDefault>{ TestDefault("a"), TestDefault("b"), TestDefault() },
        "[{\"value\":\"a\"},{\"value\":\"b\"},{\"value\":\"default\"}]"
    ));

    std::vector<TestValue> v;
    v.push_back(TestValue("a"));
    v.push_back(TestValue("b"));
    v.push_back(TestValue("c"));
    EXPECT_TRUE(test_serialize(v, "[{\"value\":\"a\"},{\"value\":\"b\"},{\"value\":\"c\"}]"));

    std::vector<std::string> val = { "one", "two" };
    EXPECT_TRUE(test_serialize(val, "[\"one\",\"two\"]"));
}

TEST(Serialization, Set)
{
    EXPECT_TRUE(test_serialize(std::set<int>(), "[]"));
    EXPECT_TRUE(test_serialize(std::set<int>{1, 2, 3}, "[1,2,3]"));
    EXPECT_TRUE(test_serialize(std::set<float>{1.5, 2.5, 3.5}, "[1.5,2.5,3.5]"));

    std::set<std::string> str_set = { "one", "two" };
    EXPECT_TRUE(test_serialize(str_set, "[\"one\",\"two\"]"));

    EXPECT_TRUE(test_serialize(
        std::set<TestDefault>{ TestDefault("a"), TestDefault("b"), TestDefault("c") },
        "[{\"value\":\"a\"},{\"value\":\"b\"},{\"value\":\"c\"}]"
    ));

    std::set<TestValue> s;
    s.insert(TestValue("a"));
    s.insert(TestValue("b"));
    s.insert(TestValue("c"));
    EXPECT_TRUE(test_serialize(s, "[{\"value\":\"a\"},{\"value\":\"b\"},{\"value\":\"c\"}]"));
}

TEST(Serialization, Map)
{
    EXPECT_TRUE(test_serialize(std::map<std::string, int>(), "{}"));

    std::map<std::string, int> int_map = {
        { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
    };
    EXPECT_TRUE(test_serialize(int_map, "{\"key1\":1,\"key2\":2,\"key3\":3}"));

    std::map<std::string, std::string> str_map = {
        { "key1", "a" }, { "key2", "b" }, { "key3", "c" }
    };
    EXPECT_TRUE(test_serialize(str_map, "{\"key1\":\"a\",\"key2\":\"b\",\"key3\":\"c\"}"));

    std::map<std::string, std::vector<int>> vec_map = {
        { "key1", {1, 2} }, { "key2", {4, 8, 16, 32} }
    };
    EXPECT_TRUE(test_serialize(vec_map, "{\"key1\":[1,2],\"key2\":[4,8,16,32]}"));

    std::map<std::string, TestValue> obj_map;
    obj_map.insert(std::make_pair("key1", TestValue("a")));
    obj_map.insert(std::make_pair("key2", TestValue("b")));
    obj_map.insert(std::make_pair("key3", TestValue("c")));
    EXPECT_TRUE(test_serialize(obj_map, "{\"key1\":{\"value\":\"a\"},\"key2\":{\"value\":\"b\"},\"key3\":{\"value\":\"c\"}}"));
}

TEST(Serialization, UnorderedMap)
{
    EXPECT_TRUE(test_serialize(std::unordered_map<std::string, int>(), "{}"));

    std::unordered_map<std::string, int> int_map = {
        { "key", 1 }
    };
    EXPECT_TRUE(test_serialize(int_map, "{\"key\":1}"));

    std::unordered_map<std::string, std::string> str_map = {
        { "key1", "a" }
    };
    EXPECT_TRUE(test_serialize(str_map, "{\"key1\":\"a\"}"));

    std::unordered_map<std::string, TestValue> obj_map;
    obj_map.insert(std::make_pair("key1", TestValue("a")));
    EXPECT_TRUE(test_serialize(obj_map, "{\"key1\":{\"value\":\"a\"}}"));
}
