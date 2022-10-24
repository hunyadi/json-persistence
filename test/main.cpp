#include <gtest/gtest.h>
#include "persistence/persistence.hpp"
#include "persistence/utility.hpp"
#include "definitions.hpp"
#include "measure.hpp"
#include "random.hpp"

using namespace persistence;
using namespace test;

template<typename T>
void test_serialize(const T& obj, const std::string& ref_json)
{
    auto&& str_json = serialize_to_string(obj);
    EXPECT_EQ(str_json, ref_json);

    rapidjson::Document doc;
    EXPECT_TRUE(serialize_to_document(obj, doc));
    auto&& doc_json = document_to_string(doc);
    EXPECT_EQ(doc_json, ref_json);
}

template<typename T>
void test_deserialize(const std::string& str, const T& ref_obj)
{
    rapidjson::Document doc = string_to_document(str);
    EXPECT_FALSE(doc.GetParseError());

    T doc_obj = deserialize<T>(doc);
    EXPECT_EQ(doc_obj, ref_obj);

    T str_obj = parse<T>(str);
    EXPECT_EQ(str_obj, ref_obj);
}

template<typename T>
void test_no_deserialize(const std::string& str)
{
    rapidjson::Document doc = string_to_document(str);
    EXPECT_FALSE(doc.GetParseError());

    T obj;
    EXPECT_FALSE(deserialize<T>(doc, obj));
    EXPECT_FALSE(parse<T>(str, obj));
}

template<typename T>
void test_integer_type()
{
    test_serialize(T(0), "0");
    test_serialize(T(1), "1");
    test_serialize(std::numeric_limits<T>::min(), std::to_string(std::numeric_limits<T>::min()));
    test_serialize(std::numeric_limits<T>::max(), std::to_string(std::numeric_limits<T>::max()));
}

template<typename T>
void test_float_type()
{
    test_serialize(T(0.0), "0.0");
    test_serialize(T(1.0), "1.0");
    test_serialize(T(-1.0), "-1.0");
    test_serialize(T(4.5), "4.5");
}

TEST(Serialization, SpecialTypes)
{
    test_serialize(nullptr, "null");
}

TEST(Serialization, BooleanTypes)
{
    test_serialize(true, "true");
    test_serialize(false, "false");
}

TEST(Serialization, NumericTypes)
{
    test_integer_type<char>();
    test_integer_type<unsigned char>();
    test_integer_type<short>();
    test_integer_type<unsigned short>();
    test_integer_type<int>();
    test_integer_type<unsigned int>();
    test_integer_type<long>();
    test_integer_type<unsigned long>();
    test_integer_type<long long>();
    test_integer_type<unsigned long long>();

    test_integer_type<int8_t>();
    test_integer_type<uint8_t>();
    test_integer_type<int16_t>();
    test_integer_type<uint16_t>();
    test_integer_type<int32_t>();
    test_integer_type<uint32_t>();
    test_integer_type<int64_t>();
    test_integer_type<uint64_t>();

    test_float_type<float>();
    test_float_type<double>();
}

TEST(Serialization, EnumTypes)
{
    test_serialize(Side::Left, "1");
    test_serialize(Side::Right, "2");

    test_serialize(Suit::Diamonds, "0");
    test_serialize(Suit::Hearts, "1");
    test_serialize(Suit::Clubs, "2");
    test_serialize(Suit::Spades, "3");
}

TEST(Serialization, StringTypes)
{
    test_serialize("", "\"\"");
    test_serialize("test string", "\"test string\"");

    test_serialize(std::string_view(), "\"\"");
    test_serialize(std::string_view("test string"), "\"test string\"");

    test_serialize(std::string(), "\"\"");
    test_serialize(std::string("test string"), "\"test string\"");
}

TEST(Serialization, DateTimeTypes)
{
    test_serialize(
        make_datetime(1982, 10, 23, 1, 20, 40),
        "\"1982-10-23T01:20:40Z\""
    );
    test_serialize(
        make_datetime(2022, 2, 1, 23, 2, 1),
        "\"2022-02-01T23:02:01Z\""
    );
}

TEST(Serialization, Pointer)
{
    auto raw_pointer = new TestValue("string");
    test_serialize(raw_pointer, "{\"value\":\"string\"}");
    delete raw_pointer;

    auto unique_pointer = std::make_unique<TestValue>("string");
    test_serialize(unique_pointer, "{\"value\":\"string\"}");

    auto shared_pointer = std::make_shared<TestValue>("string");
    test_serialize(shared_pointer, "{\"value\":\"string\"}");
}

TEST(Serialization, Pair)
{
    test_serialize(
        std::make_pair(19, std::string("eighty-two")),
        "[19,\"eighty-two\"]"
    );
}

TEST(Serialization, Tuple)
{
    test_serialize(std::tuple<>(), "[]");
    test_serialize(
        std::make_tuple(std::string("value")),
        "[\"value\"]"
    );
    test_serialize(
        std::make_tuple(19, std::string("eighty-two")),
        "[19,\"eighty-two\"]"
    );
}

TEST(Serialization, Variant)
{
    using variant_type = std::variant<int, std::string>;
    test_serialize(variant_type("value"), "\"value\"");
    test_serialize(variant_type(23), "23");
}

TEST(Serialization, Vector)
{
    test_serialize(std::vector<int>(), "[]");
    test_serialize(std::vector<int>{1, 2, 3}, "[1,2,3]");
    test_serialize(std::vector<std::vector<int>>{ { 1 }, { 1,2 }, { 1,2,3 } }, "[[1],[1,2],[1,2,3]]");

    std::vector<std::string> val = { "one", "two" };
    test_serialize(val, "[\"one\",\"two\"]");
}

TEST(Serialization, Array)
{
    test_serialize(std::array<int, 0>(), "[]");
    test_serialize(std::array<int, 3>{1, 2, 3}, "[1,2,3]");

    std::array<std::string, 2> val = { "one", "two" };
    test_serialize(val, "[\"one\",\"two\"]");
}

TEST(Serialization, Set)
{
    test_serialize(std::set<int>(), "[]");
    test_serialize(std::set<int>{1, 2, 3}, "[1,2,3]");

    std::set<std::string> val = { "one", "two" };
    test_serialize(val, "[\"one\",\"two\"]");
}

TEST(Serialization, Map)
{
    test_serialize(std::map<std::string, int>(), "{}");

    std::map<std::string, int> val = {
        { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
    };
    test_serialize(val, "{\"key1\":1,\"key2\":2,\"key3\":3}");
}

TEST(Serialization, UnorderedMap)
{
    test_serialize(std::unordered_map<std::string, int>(), "{}");

    std::unordered_map<std::string, int> val = {
        { "key", 1 }
    };
    test_serialize(val, "{\"key\":1}");
}

TEST(Serialization, Object)
{
    TestValue value_object("test string");
    test_serialize(value_object, "{\"value\":\"test string\"}");

    TestDataTransferObject dto;
    dto.bool_value = true;
    dto.bool_list = { false, false, true };
    dto.int_value = 42;
    dto.int_list = { 82, 10, 23 };
    dto.string_value = "test string";
    dto.string_list = { "one","two","three" };
    TestDataTransferSubObject o;
    o.member_value = "value";
    dto.object_value = o;
    dto.object_list = { o, o };
    test_serialize(dto,
        "{\"bool_value\":true,"
        "\"bool_list\":[false,false,true],"
        "\"int_value\":42,"
        "\"int_list\":[82,10,23],"
        "\"string_value\":\"test string\","
        "\"string_list\":[\"one\",\"two\",\"three\"],"
        "\"object_value\":{\"member_value\":\"value\"},"
        "\"object_list\":[{\"member_value\":\"value\"},{\"member_value\":\"value\"}]}"
    );
}

TEST(Serialization, Optional)
{
    TestOptionalObjectMember opt1;
    test_serialize(opt1, "{}");

    TestOptionalObjectMember opt2;
    opt2.optional_value = 42;
    test_serialize(opt2, "{\"optional_value\":42}");
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
    test_serialize(obj, "{"
        "\"values\":["
            "{\"value\":\"one\"},"
            "{\"value\":\"two\"},"
            "{\"$ref\":\"/values/1\"},"
            "{\"$ref\":\"/values/1\"}"
        "]"
    "}");
}

TEST(Serialization, BackReferenceObject)
{
    TestBackReferenceObject obj;
    obj.outer = std::make_shared<TestValue>("string");
    obj.inner.push_back(obj.outer);
    obj.inner.push_back(obj.outer);
    test_serialize(obj, "{"
        "\"outer\":{\"value\":\"string\"},"
        "\"inner\":[{\"$ref\":\"/outer\"},{\"$ref\":\"/outer\"}]"
    "}");
}

TEST(Deserialization, SpecialTypes)
{
    test_deserialize("null", nullptr);
}

TEST(Deserialization, BooleanTypes)
{
    test_deserialize("true", true);
    test_deserialize("false", false);

    test_no_deserialize<bool>("0");
    test_no_deserialize<bool>("1");
}

TEST(Deserialization, NumericTypes)
{
    test_deserialize("127", static_cast<char>(127));
    test_deserialize("127", static_cast<unsigned char>(127));

    test_deserialize("1024", 1024);
    test_deserialize("-1024", -1024);
    test_deserialize("1024", 1024u);

    test_deserialize("2048", 2048l);
    test_deserialize("-2048", -2048l);
    test_deserialize("2048", 2048ul);

    test_deserialize("4096", 4096ll);
    test_deserialize("-4096", -4096ll);
    test_deserialize("4096", 4096ull);

    test_deserialize("9.5", 9.5f);
    test_deserialize("1.25", 1.25);

    test_no_deserialize<int>("1.0");
    test_no_deserialize<int>("-1.0");
}

TEST(Deserialization, EnumTypes)
{
    test_deserialize("1", Side::Left);
    test_deserialize("2", Side::Right);

    test_deserialize("0", Suit::Diamonds);
    test_deserialize("1", Suit::Hearts);
    test_deserialize("2", Suit::Clubs);
    test_deserialize("3", Suit::Spades);
}

TEST(Deserialization, StringTypes)
{
    test_deserialize<std::string>("\"\"", std::string());
    test_deserialize<std::string>("\"test string\"", std::string("test string"));
}

TEST(Deserialization, DateTimeTypes)
{
    test_deserialize(
        "\"1982-10-23T01:20:40Z\"",
        make_datetime(1982, 10, 23, 1, 20, 40)
    );
    test_deserialize(
        "\"2022-02-01T23:02:01Z\"",
        make_datetime(2022, 2, 1, 23, 2, 1)
    );

    // space instead of 'T'
    test_deserialize(
        "\"2022-02-01 23:02:01Z\"",
        make_datetime(2022, 2, 1, 23, 2, 1)
    );

    // two-digit year
    test_no_deserialize<timestamp>("\"22-02-01T23:02:01Z\"");

    // missing or wrong time zone
    test_no_deserialize<timestamp>("\"2022-02-01T23:02:01\"");
    test_no_deserialize<timestamp>("\"2022-02-01T23:02:01A\"");

    // invalid year, month, day, hour, minute or second
    test_no_deserialize<timestamp>("\"YYYY-02-01T23:02:01Z\"");
    test_no_deserialize<timestamp>("\"2022-MM-01T23:02:01Z\"");
    test_no_deserialize<timestamp>("\"2022-02-DDT23:02:01Z\"");
    test_no_deserialize<timestamp>("\"2022-02-01THH:02:01Z\"");
    test_no_deserialize<timestamp>("\"2022-02-01T23:MM:01Z\"");
    test_no_deserialize<timestamp>("\"2022-02-01T23:02:SSZ\"");
}

TEST(Deserialization, Pointer)
{
    EXPECT_EQ(*deserialize<std::unique_ptr<TestValue>>("{\"value\": \"string\"}"), TestValue("string"));
    EXPECT_EQ(*deserialize<std::shared_ptr<TestValue>>("{\"value\": \"string\"}"), TestValue("string"));
}

TEST(Deserialization, Pair)
{
    test_deserialize(
        "[19, \"eighty-two\"]",
        std::make_pair(19, std::string("eighty-two"))
    );
}

TEST(Deserialization, Tuple)
{
    test_deserialize("[]", std::tuple<>());
    test_deserialize("[\"value\"]", std::make_tuple(std::string("value")));
    test_deserialize(
        "[19, \"eighty-two\"]",
        std::make_tuple(19, std::string("eighty-two"))
    );
}

TEST(Deserialization, Variant)
{
    using variant_type = std::variant<int, std::string>;

    EXPECT_EQ(deserialize<variant_type>("\"value\""), variant_type(std::string("value")));
    EXPECT_EQ(deserialize<variant_type>("23"), variant_type(23));

    variant_type result;
    EXPECT_FALSE(deserialize<variant_type>("", result));
    EXPECT_FALSE(deserialize<variant_type>("null", result));
    EXPECT_FALSE(deserialize<variant_type>("true", result));
    EXPECT_FALSE(deserialize<variant_type>("4.5", result));
    EXPECT_FALSE(deserialize<variant_type>("[23]", result));
    EXPECT_FALSE(deserialize<variant_type>("{}", result));
}

TEST(Deserialization, Vector)
{
    test_deserialize("[]", std::vector<int>());
    test_deserialize("[1, 2, 3]", std::vector<int> { 1, 2, 3 });
    test_deserialize("[\"one\", \"two\"]", std::vector<std::string> { "one", "two" });

    std::vector<std::vector<int>> ref_nested = { { 1 }, { 1,2 }, { 1,2,3 } };
    test_deserialize("[[1], [1, 2], [1, 2, 3]]", ref_nested);
}

TEST(Deserialization, Array)
{
    test_deserialize("[]", std::array<int, 0> {});
    test_deserialize("[1, 2, 3]", std::array<int, 3> { 1, 2, 3 });
    test_deserialize("[\"one\", \"two\"]", std::array<std::string, 2> { "one", "two" });
}

TEST(Deserialization, Set)
{
    test_deserialize("[]", std::set<int>());
    test_deserialize("[]", std::set<std::string>());

    test_deserialize(
        "[1, 2, 3]",
        std::set<int> { 1, 2, 3 }
    );
    test_deserialize(
        "[\"one\", \"two\"]",
        std::set<std::string> { "one", "two" }
    );
}

TEST(Deserialization, Map)
{
    test_deserialize("{}", std::map<std::string, int>());
    test_deserialize("{}", std::map<std::string, std::string>());

    test_deserialize(
        "{\"key1\": 1, \"key2\": 2, \"key3\": 3}",
        std::map<std::string, int> {
            { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
        }
    );
    test_deserialize(
        "{\"key1\": \"value1\", \"key2\": \"value2\"}",
        std::map<std::string, std::string> {
            { "key1", "value1" }, { "key2", "value2" }
        }
    );
}

TEST(Deserialization, UnorderedMap)
{
    test_deserialize("{}", std::unordered_map<std::string, int>());
    test_deserialize("{}", std::unordered_map<std::string, std::string>());

    test_deserialize(
        "{\"key1\": 1, \"key2\": 2, \"key3\": 3}",
        std::unordered_map<std::string, int> {
            { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
        }
    );
    test_deserialize(
        "{\"key1\": \"value1\", \"key2\": \"value2\"}",
        std::unordered_map<std::string, std::string> {
            { "key1", "value1" }, { "key2", "value2" }
        }
    );
}

TEST(Deserialization, Object)
{
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
    o.member_value = "value";
    ref.object_value = o;
    ref.object_list = { o, o };
    test_deserialize(json, ref);

    test_deserialize("{\"member\":{\"value\": \"string\"}}", TestNonCopyable("string"));
}

TEST(Deserialization, Optional)
{
    test_deserialize("{}", TestOptionalObjectMember());
    test_deserialize("{\"optional_value\": 42}", TestOptionalObjectMember(42));
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

    auto val = deserialize<TestBackReferenceArray>(std::move(json));
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

    auto val = deserialize<TestBackReferenceObject>(std::move(json));
    EXPECT_EQ(val.outer, val.inner[0]);
    EXPECT_EQ(val.outer, val.inner[1]);
}

#ifndef _DEBUG
TEST(Performance, Object)
{
    std::default_random_engine engine;

    std::vector<TestDataTransferObject> items;
    for (std::size_t k = 0; k < 1000000; ++k) {
        TestDataTransferObject item;
        item.bool_value = random_bool(engine);
        item.bool_list = random_items(engine, [](std::default_random_engine& engine) { return random_bool(engine); }, 0, 100);
        item.int_value = random_integer<int>(engine);
        item.int_list = random_items(engine, [](std::default_random_engine& engine) { return random_integer<int>(engine); }, 0, 100);
        item.string_value = "test string";
        item.string_list = { "one","two","three" };
        items.push_back(item);
    }

    auto&& json = measure("serialize from object to string", [&]() {
        return serialize_to_string(items);
    });
    std::cout << "JSON string has size of " << json.size() << " B" << std::endl;
    measure("serialize from object to DOM", [&]() {
        rapidjson::Document doc;
        serialize_to_document(items, doc);
    });
    measure("deserialize from string to DOM", [&]() {
        rapidjson::Document doc;
        doc.Parse(json.data());
    });
    measure("deserialize from string to object via DOM", [&]() {
        deserialize<std::vector<TestDataTransferObject>>(json);
    });
    auto&& values = measure("deserialize from string to object", [&]() {
        return parse<std::vector<TestDataTransferObject>>(json);
    });
    EXPECT_GT(values.size(), 0);
}
#endif
