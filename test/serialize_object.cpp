#include <gtest/gtest.h>
#include "persistence/write_object.hpp"
#include "persistence/write_pointer.hpp"
#include "persistence/write_fundamental.hpp"
#include "persistence/write_string.hpp"
#include "persistence/write_vector.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_object.hpp"
#include "persistence/serialize_pointer.hpp"
#include "persistence/serialize_fundamental.hpp"
#include "persistence/serialize_string.hpp"
#include "persistence/serialize_vector.hpp"
#include "persistence/serialize.hpp"
#include "example_classes.hpp"
#include "test_serialize.hpp"

using namespace test;

TEST(Serialization, Pointer)
{
    EXPECT_TRUE(test_serialize(std::make_unique<int>(23), "23"));
    EXPECT_TRUE(test_serialize(std::make_shared<int>(23), "23"));
    EXPECT_TRUE(test_serialize(std::make_unique<std::string>("string"), "\"string\""));
    EXPECT_TRUE(test_serialize(std::make_shared<std::string>("string"), "\"string\""));

    auto raw_pointer = new TestValue("string");
    EXPECT_TRUE(test_serialize(raw_pointer, "{\"value\":\"string\"}"));
    delete raw_pointer;

    auto unique_pointer = std::make_unique<TestValue>("string");
    EXPECT_TRUE(test_serialize(unique_pointer, "{\"value\":\"string\"}"));

    auto shared_pointer = std::make_shared<TestValue>("string");
    EXPECT_TRUE(test_serialize(shared_pointer, "{\"value\":\"string\"}"));
}

TEST(Serialization, Object)
{
    TestValue value_object("test string");
    EXPECT_TRUE(test_serialize(value_object, "{\"value\":\"test string\"}"));

    TestDefault default_object;
    EXPECT_TRUE(test_serialize(default_object, "{\"value\":\"default\"}"));

    TestDerived derived_object("base", "derived");
    EXPECT_TRUE(test_serialize(derived_object, "{\"value\":\"base\",\"member\":\"derived\"}"));

    TestDataTransferObject dto;
    dto.bool_value = true;
    dto.bool_list = { false, false, true };
    dto.int_value = 42;
    dto.int_list = { 82, 10, 23 };
    dto.string_value = "test string";
    dto.string_list = { "one","two","three" };
    TestDataTransferSubObject o;
    o.value = "value";
    dto.object_value = o;
    dto.object_list = { o, o };
    const char* json = "{"
        "\"bool_value\":true,"
        "\"bool_list\":[false,false,true],"
        "\"int_value\":42,"
        "\"int_list\":[82,10,23],"
        "\"string_value\":\"test string\","
        "\"string_list\":[\"one\",\"two\",\"three\"],"
        "\"object_value\":{\"member_value\":\"value\"},"
        "\"object_list\":[{\"member_value\":\"value\"},{\"member_value\":\"value\"}]"
    "}";
    EXPECT_TRUE(test_serialize(dto, json));
}

TEST(Serialization, Optional)
{
    TestOptionalObjectMember opt1;
    EXPECT_TRUE(test_serialize(opt1, "{}"));

    TestOptionalObjectMember opt2;
    opt2.optional_value = 42;
    EXPECT_TRUE(test_serialize(opt2, "{\"optional_value\":42}"));
}

TEST(Serialization, BackReferenceArray)
{
    auto val = std::make_shared<TestValue>("one");
    auto rep = std::make_shared<TestValue>("two");
    TestBackReferenceArray obj;
    obj.values.push_back(val);
    obj.values.push_back(rep);
    obj.values.push_back(rep);
    obj.values.push_back(rep);
    const char* json = "{"
        "\"values\":["
            "{\"value\":\"one\"},"
            "{\"value\":\"two\"},"
            "{\"$ref\":\"/values/1\"},"
            "{\"$ref\":\"/values/1\"}"
        "]"
    "}";
    EXPECT_TRUE(test_serialize(obj, json));
}

TEST(Serialization, BackReferenceObject)
{
    TestBackReferenceObject obj;
    obj.outer = std::make_shared<TestValue>("string");
    obj.inner.push_back(obj.outer);
    obj.inner.push_back(obj.outer);
    const char* json = "{"
        "\"outer\":{\"value\":\"string\"},"
        "\"inner\":["
            "{\"$ref\":\"/outer\"},"
            "{\"$ref\":\"/outer\"}"
        "]"
    "}";
    EXPECT_TRUE(test_serialize(obj, json));
}
