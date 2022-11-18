#include <gtest/gtest.h>
#include "persistence/write_string.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_string.hpp"
#include "persistence/serialize.hpp"
#include "string.hpp"
#include "test_serialize.hpp"

using namespace test;

TEST(Serialization, StringTypes)
{
    EXPECT_TRUE(test_serialize("", "\"\""));
    EXPECT_TRUE(test_serialize("test string", "\"test string\""));

    EXPECT_TRUE(test_serialize(std::string_view(), "\"\""));
    EXPECT_TRUE(test_serialize(std::string_view("test string"), "\"test string\""));
    EXPECT_TRUE(test_serialize(std::string_view(view_from_chars("test\0string")), "\"test\\u0000string\""));

    EXPECT_TRUE(test_serialize(std::string(), "\"\""));
    EXPECT_TRUE(test_serialize(std::string("test string"), "\"test string\""));

    // NUL byte in middle of string
    EXPECT_TRUE(test_serialize(std::string(view_from_chars("test\0string")), "\"test\\u0000string\""));

    // UTF-8 characters
    EXPECT_TRUE(test_serialize(std::string("árvíztűrő tükörfúrógép"), "\"árvíztűrő tükörfúrógép\""));
}
