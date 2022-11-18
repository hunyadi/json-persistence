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
    EXPECT_TRUE(test_serialize(std::vector<std::vector<int>>{ { 1 }, { 1,2 }, { 1,2,3 } }, "[[1],[1,2],[1,2,3]]"));

    EXPECT_TRUE(test_serialize(
        std::vector<TestDefault>{ TestDefault("a"), TestDefault("b"), TestDefault("c") },
        "[{\"value\":\"a\"},{\"value\":\"b\"},{\"value\":\"c\"}]"
    ));

    std::vector<std::string> val = { "one", "two" };
    EXPECT_TRUE(test_serialize(val, "[\"one\",\"two\"]"));
}

TEST(Serialization, Set)
{
    EXPECT_TRUE(test_serialize(std::set<int>(), "[]"));
    EXPECT_TRUE(test_serialize(std::set<int>{1, 2, 3}, "[1,2,3]"));

    EXPECT_TRUE(test_serialize(
        std::set<TestDefault>{ TestDefault("a"), TestDefault("b"), TestDefault("c") },
        "[{\"value\":\"a\"},{\"value\":\"b\"},{\"value\":\"c\"}]"
    ));

    std::set<std::string> val = { "one", "two" };
    EXPECT_TRUE(test_serialize(val, "[\"one\",\"two\"]"));
}

TEST(Serialization, Map)
{
    EXPECT_TRUE(test_serialize(std::map<std::string, int>(), "{}"));

    std::map<std::string, int> val = {
        { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
    };
    EXPECT_TRUE(test_serialize(val, "{\"key1\":1,\"key2\":2,\"key3\":3}"));
}

TEST(Serialization, UnorderedMap)
{
    EXPECT_TRUE(test_serialize(std::unordered_map<std::string, int>(), "{}"));

    std::unordered_map<std::string, int> val = {
        { "key", 1 }
    };
    EXPECT_TRUE(test_serialize(val, "{\"key\":1}"));
}
