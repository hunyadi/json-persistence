#include <gtest/gtest.h>
#include "persistence/write_variant.hpp"
#include "persistence/write_fundamental.hpp"
#include "persistence/write_object.hpp"
#include "persistence/write_string.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_variant.hpp"
#include "persistence/serialize_fundamental.hpp"
#include "persistence/serialize_object.hpp"
#include "persistence/serialize_string.hpp"
#include "persistence/serialize.hpp"
#include "example_classes.hpp"
#include "test_serialize.hpp"

using namespace test;

TEST(Serialization, Variant)
{
    using simple_variant_type = std::variant<int, std::string>;
    EXPECT_TRUE(test_serialize(simple_variant_type("value"), "\"value\""));
    EXPECT_TRUE(test_serialize(simple_variant_type(23), "23"));

    using mixed_variant_type = std::variant<std::string, TestValue>;
    EXPECT_TRUE(test_serialize(mixed_variant_type("value"), "\"value\""));
    EXPECT_TRUE(test_serialize(mixed_variant_type(TestValue("string")), "{\"value\":\"string\"}"));

    using complex_variant_type = std::variant<TestValue, TestNonCopyable>;
    EXPECT_TRUE(test_serialize(complex_variant_type(TestValue("string")), "{\"value\":\"string\"}"));
    EXPECT_TRUE(test_serialize(complex_variant_type(TestNonCopyable("string")), "{\"member\":{\"value\":\"string\"}}"));
}
