#include <gtest/gtest.h>
#include "persistence/parse_string.hpp"
#include "persistence/parse.hpp"
#include "persistence/deserialize_string.hpp"
#include "persistence/deserialize.hpp"
#include "string.hpp"
#include "test_deserialize.hpp"

using namespace test;

TEST(Deserialization, StringTypes)
{
    EXPECT_TRUE(test_deserialize("\"\"", std::string()));
    EXPECT_TRUE(test_deserialize("\"test string\"", std::string("test string")));

    // NUL byte in middle of string
    EXPECT_TRUE(test_deserialize("\"test\\u0000string\"", std::string(view_from_chars("test\0string"))));

    // UTF-8 characters
    EXPECT_TRUE(test_deserialize("\"árvíztűrő tükörfúrógép\"", std::string("árvíztűrő tükörfúrógép")));

    EXPECT_TRUE(test_no_deserialize<std::string>("true"));
    EXPECT_TRUE(test_no_deserialize<std::string>("23"));
    EXPECT_TRUE(test_no_deserialize<std::string>("[]"));
    EXPECT_TRUE(test_no_deserialize<std::string>("{}"));
}
