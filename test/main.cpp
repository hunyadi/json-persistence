#include <gtest/gtest.h>
#include "persistence/persistence.hpp"
#include "persistence/utility.hpp"
#include "example_classes.hpp"
#include "measure.hpp"
#include "random.hpp"
#include "test_serialize.hpp"
#include "test_deserialize.hpp"

using namespace persistence;
using namespace test;

TEST(Deserialization, ErrorReporting)
{
    try {
        parse<Example>("{\"bool_value\": true, []}");
        FAIL();
    } catch (JsonParseError& e) {
        EXPECT_EQ(e.offset, 21);
    }

    try {
        parse<Example>("{\"bool_value\": true, \"string_value\": []}");
        FAIL();
    } catch (JsonParseError& e) {
        EXPECT_EQ(e.offset, 38);
    }

    try {
        deserialize<Example>(
            "{"
                "\"bool_value\": true,"
                "\"string_value\": \"\","
                "\"string_list\": [\"a\",23]"
            "}"
        );
        FAIL();
    } catch (JsonDeserializationError& e) {
        EXPECT_EQ(e.path, "/string_list/1");
    }
}

#ifndef _DEBUG
TEST(Performance, Object)
{
    std::default_random_engine engine;

    std::vector<TestDataTransferObject> items;
    for (std::size_t k = 0; k < 100000; ++k) {
        TestDataTransferObject item;
        item.bool_value = random_bool(engine);
        item.bool_list = random_items(engine, [](std::default_random_engine& engine) { return random_bool(engine); }, 0, 100);
        item.int_value = random_integer<int>(engine);
        item.int_list = random_items(engine, [](std::default_random_engine& engine) { return random_integer<int>(engine); }, 0, 100);
        item.string_value = "test string";
        item.string_list = { "one","two","three" };
        items.push_back(item);
    }

    auto json = measure("write object to string", [&] {
        return write_to_string(items);
    });
    EXPECT_GT(json.size(), 0);
    std::cout << "JSON string has size of " << json.size() << " B" << std::endl;
    measure("parse object from string", [&] {
        parse<std::vector<TestDataTransferObject>>(json);
    });

    auto doc = measure("deserialize DOM from string", [&] {
        return string_to_document(json);
    });
    EXPECT_FALSE(doc.HasParseError());
    measure("serialize DOM to string", [&] {
        document_to_string(doc);
    });

    measure("serialize object to DOM", [&] {
        serialize_to_document(items);
    });
    measure("deserialize object from DOM with exceptions disabled", [&] {
        std::vector<TestDataTransferObject> obj;
        deserialize(doc, obj);
    });
    measure("deserialize object from DOM with exceptions enabled", [&] {
        deserialize<std::vector<TestDataTransferObject>>(doc);
    });

    measure("deserialize object from string via DOM with exceptions disabled", [&] {
        std::vector<TestDataTransferObject> obj;
        deserialize(json, obj);
    });
    measure("deserialize object from string via DOM with exceptions enabled", [&] {
        deserialize<std::vector<TestDataTransferObject>>(json);
    });
}
#endif

TEST(Documentation, Example)
{
    Example obj = { true, "string", {"a","b","c"}, std::nullopt, {"xyz"} };
    auto json = write_to_string(obj);
    Example res = parse<Example>(json);

    EXPECT_EQ(write_to_string(Derived()), "{\"base_member\":\"base\",\"derived_member\":\"derived\"}");
}
