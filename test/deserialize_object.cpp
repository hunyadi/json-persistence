#include <gtest/gtest.h>
#include "persistence/parse_object.hpp"
#include "persistence/parse_pointer.hpp"
#include "persistence/parse_fundamental.hpp"
#include "persistence/parse_string.hpp"
#include "persistence/parse_vector.hpp"
#include "persistence/parse.hpp"
#include "persistence/deserialize_object.hpp"
#include "persistence/deserialize_pointer.hpp"
#include "persistence/deserialize_fundamental.hpp"
#include "persistence/deserialize_string.hpp"
#include "persistence/deserialize_vector.hpp"
#include "persistence/deserialize.hpp"
#include "example_classes.hpp"
#include "test_deserialize.hpp"

using namespace test;

TEST(Deserialization, Pointer)
{
    EXPECT_TRUE(test_deserialize("23", std::make_unique<int>(23)));
    EXPECT_TRUE(test_deserialize("23", std::make_shared<int>(23)));
    EXPECT_TRUE(test_deserialize("\"string\"", std::make_unique<std::string>("string")));
    EXPECT_TRUE(test_deserialize("\"string\"", std::make_shared<std::string>("string")));
    EXPECT_TRUE(test_deserialize("{\"value\": \"string\"}", std::make_unique<TestValue>("string")));
    EXPECT_TRUE(test_deserialize("{\"value\": \"string\"}", std::make_shared<TestValue>("string")));
    EXPECT_TRUE(test_deserialize("{}", std::make_shared<TestDefault>()));
}

TEST(Deserialization, Object)
{
    EXPECT_TRUE(test_deserialize("{\"value\":\"test string\"}", TestValue("test string")));
    EXPECT_TRUE(test_deserialize("{}", TestDefault("default")));
    EXPECT_TRUE(test_deserialize("{\"value\":\"test string\"}", TestDefault("test string")));
    EXPECT_TRUE(test_deserialize("{\"value\":\"base\",\"member\":\"derived\"}", TestDerived("base", "derived")));

    const char* json =
        "{"
        "\"bool_value\":true,"
        "\"bool_list\":[false,false,true],"
        "\"int_value\":42,"
        "\"int_list\":[82,10,23],"
        "\"string_value\":\"test string\","
        "\"string_list\":[\"one\",\"two\",\"three\"],"
        "\"object_value\":{\"member_value\":\"value\"},"
        "\"object_list\":[{\"member_value\":\"value\"},{\"member_value\":\"value\"}]"
        "}"
    ;

    TestDataTransferObject ref;
    ref.bool_value = true;
    ref.bool_list = { false, false, true };
    ref.int_value = 42;
    ref.int_list = { 82, 10, 23 };
    ref.string_value = "test string";
    ref.string_list = { "one","two","three" };
    TestDataTransferSubObject o;
    o.value = "value";
    ref.object_value = o;
    ref.object_list = { o, o };
    EXPECT_TRUE(test_deserialize(json, ref));

    EXPECT_TRUE(test_deserialize("{\"member\": {\"value\": \"string\"}}", TestNonCopyable("string")));
    EXPECT_TRUE(test_no_deserialize<TestNonCopyable>("{\"member\": {\"value\": 23}}"));
}

TEST(Deserialization, Optional)
{
    EXPECT_TRUE(test_deserialize("{}", TestOptionalObjectMember()));
    EXPECT_TRUE(test_deserialize("{\"optional_value\": 42}", TestOptionalObjectMember(42)));
}

TEST(Deserialization, BackReferenceArray)
{
    std::string json =
        "{\"values\":["
            "{\"value\":\"string\"},"
            "{\"value\":\"string\"},"
            "{\"$ref\":\"/values/1\"},"
            "{\"$ref\":\"/values/1\"}"
        "]}";

    auto val = persistence::deserialize<TestBackReferenceArray>(std::move(json));
    EXPECT_NE(val.values[0], val.values[1]);
    EXPECT_EQ(val.values[1], val.values[2]);
    EXPECT_EQ(val.values[2], val.values[3]);
}

TEST(Deserialization, BackReferenceObject)
{
    std::string json =
        "{"
            "\"outer\": {\"value\": \"string\"},"
            "\"inner\": ["
                "{\"$ref\": \"/outer\"},"
                "{\"$ref\": \"/outer\"}"
            "]"
        "}";

    auto obj = persistence::deserialize<TestBackReferenceObject>(json);
    EXPECT_EQ(obj.outer, obj.inner[0]);
    EXPECT_EQ(obj.outer, obj.inner[1]);

    // back-references are not supported by the parser
    EXPECT_THROW(persistence::parse<TestBackReferenceObject>(json), persistence::JsonParseError);
}
