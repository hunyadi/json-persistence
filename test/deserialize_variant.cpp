#include <gtest/gtest.h>
#include "persistence/deserialize_variant.hpp"
#include "persistence/deserialize_fundamental.hpp"
#include "persistence/deserialize_object.hpp"
#include "persistence/deserialize_string.hpp"
#include "persistence/deserialize.hpp"
#include "example_classes.hpp"
#include "test_deserialize.hpp"

using namespace test;

TEST(Deserialization, Variant)
{
    using persistence::deserialize;

    using simple_variant_type = std::variant<int, std::string>;
    EXPECT_EQ(deserialize<simple_variant_type>("\"value\""), simple_variant_type(std::string("value")));
    EXPECT_EQ(deserialize<simple_variant_type>("23"), simple_variant_type(23));

    simple_variant_type result;
    EXPECT_FALSE(deserialize<simple_variant_type>("", result));
    EXPECT_FALSE(deserialize<simple_variant_type>("null", result));
    EXPECT_FALSE(deserialize<simple_variant_type>("true", result));
    EXPECT_FALSE(deserialize<simple_variant_type>("4.5", result));
    EXPECT_FALSE(deserialize<simple_variant_type>("[23]", result));
    EXPECT_FALSE(deserialize<simple_variant_type>("{}", result));

    using mixed_variant_type = std::variant<std::string, TestValue>;
    EXPECT_EQ(deserialize<mixed_variant_type>("\"value\""), mixed_variant_type(std::string("value")));
    EXPECT_EQ(deserialize<mixed_variant_type>("{\"value\": \"string\"}"), mixed_variant_type(TestValue("string")));

    using complex_variant_type = std::variant<TestValue, TestNonCopyable>;
    EXPECT_EQ(deserialize<complex_variant_type>("{\"value\": \"string\"}"), complex_variant_type(TestValue("string")));
    EXPECT_EQ(deserialize<complex_variant_type>("{\"member\":{\"value\": \"string\"}}"), complex_variant_type(TestNonCopyable("string")));
}
