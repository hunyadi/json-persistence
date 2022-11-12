#include <gtest/gtest.h>
#include "persistence/persistence.hpp"
#include "persistence/schema_utility.hpp"
#include "persistence/utility.hpp"
#include "definitions.hpp"
#include "enumerations.hpp"
#include "measure.hpp"
#include "random.hpp"
#include "string.hpp"

using namespace persistence;
using namespace test;

/**
* Serializes an object using various methods, and compares the result to a reference value.
*
* @param obj The C++ object to serialize.
* @param ref_json The reference JSON string to compare against.
*/
template<typename T>
testing::AssertionResult test_serialize(const T& obj, const std::string& ref_json)
{
    bool result;

    rapidjson::Document doc;
    result = serialize_to_document(obj, doc);
    if (result) {
        auto&& doc_json = document_to_string(doc);
        EXPECT_EQ(doc_json, ref_json);
        result = (doc_json == ref_json);
    }
    if (!result) {
        return testing::AssertionFailure() << "serialization failed";
    }

    std::string str_json;
    result = write_to_string(obj, str_json);
    if (result) {
        EXPECT_EQ(str_json, ref_json);
        EXPECT_EQ(write_to_string(obj), ref_json);
        result = (str_json == ref_json);
    }
    if (!result) {
        return testing::AssertionFailure() << "writing to JSON failed";
    }

    return testing::AssertionSuccess();
}

template<typename T>
testing::AssertionResult test_serialize(const T& obj, const std::string& ref_json, bool& outcome)
{
    auto result = test_serialize(obj, ref_json);
    outcome = outcome && !!result;
    return result;
}

/**
* De-serializes an object using various methods, and compares the result to a reference value.
*
* @param obj The JSON string to de-serialize.
* @param ref_json The reference C++ object to compare against.
*/
template<typename T>
testing::AssertionResult test_deserialize(const std::string& str, const T& ref_obj)
{
    bool result;

    rapidjson::Document doc = string_to_document(str);
    if (doc.HasParseError()) {
        return testing::AssertionFailure() << "document parse error";
    }

    T doc_obj;
    result = deserialize(doc, doc_obj);
    if (result) {
        if constexpr (is_pointer_like_v<T>) {
            EXPECT_EQ(*doc_obj, *ref_obj);
        } else {
            EXPECT_EQ(doc_obj, ref_obj);
        }
        doc_obj = deserialize<T>(doc);
        if constexpr (is_pointer_like_v<T>) {
            EXPECT_EQ(*doc_obj, *ref_obj);
            result = (*doc_obj == *ref_obj);
        } else {
            EXPECT_EQ(doc_obj, ref_obj);
            result = (doc_obj == ref_obj);
        }
    }
    if (!result) {
        return testing::AssertionFailure() << "de-serialization failed";
    }

    T str_obj;
    result = parse(str, str_obj);
    if (result) {
        if constexpr (is_pointer_like_v<T>) {
            EXPECT_EQ(*str_obj, *ref_obj);
        } else {
            EXPECT_EQ(str_obj, ref_obj);
        }
        str_obj = parse<T>(str);
        if constexpr (is_pointer_like_v<T>) {
            EXPECT_EQ(*str_obj, *ref_obj);
            result = (*str_obj == *ref_obj);
        } else {
            EXPECT_EQ(str_obj, ref_obj);
            result = (str_obj == ref_obj);
        }
    }
    if (!result) {
        return testing::AssertionFailure() << "parse from JSON failed";
    }

    return testing::AssertionSuccess();
}

template<typename T>
testing::AssertionResult test_deserialize(const std::string& str, const T& ref_obj, bool& outcome)
{
    auto result = test_deserialize(str, ref_obj);
    outcome = outcome && !!result;
    return result;
}

template<typename T>
testing::AssertionResult test_no_deserialize(const std::string& str)
{
    rapidjson::Document doc = string_to_document(str);
    if (doc.HasParseError()) {
        return testing::AssertionFailure() << "document parse error";
    }

    T obj;
    if (deserialize<T>(doc, obj)) {
        return testing::AssertionFailure() << "de-serialization succeeded unexpectedly";
    }
    EXPECT_THROW(deserialize<T>(doc), JsonDeserializationError);

    if (parse<T>(str, obj)) {
        return testing::AssertionFailure() << "parsing from JSON succeeded unexpectedly";
    }
    EXPECT_THROW(parse<T>(str), JsonParseError);

    return testing::AssertionSuccess();
}

template<typename T>
testing::AssertionResult test_no_deserialize(const std::string& str, bool& outcome)
{
    auto result = test_no_deserialize<T>(str);
    outcome = outcome && !!result;
    return result;
}

TEST(Serialization, SpecialTypes)
{
    EXPECT_TRUE(test_serialize(nullptr, "null"));
}

TEST(Serialization, BooleanTypes)
{
    EXPECT_TRUE(test_serialize(true, "true"));
    EXPECT_TRUE(test_serialize(false, "false"));
}

template<typename T>
testing::AssertionResult test_serialize_integer_type()
{
    bool outcome = true;
    EXPECT_TRUE(test_serialize(T(0), "0", outcome));
    EXPECT_TRUE(test_serialize(T(1), "1", outcome));
    EXPECT_TRUE(test_serialize(std::numeric_limits<T>::min(), std::to_string(std::numeric_limits<T>::min()), outcome));
    EXPECT_TRUE(test_serialize(std::numeric_limits<T>::max(), std::to_string(std::numeric_limits<T>::max()), outcome));

    if (outcome) {
        return testing::AssertionSuccess();
    } else {
        return testing::AssertionFailure();
    }
}

template<typename T>
testing::AssertionResult test_serialize_float_type()
{
    bool outcome = true;
    EXPECT_TRUE(test_serialize(T(0.0), "0.0", outcome));
    EXPECT_TRUE(test_serialize(T(1.0), "1.0", outcome));
    EXPECT_TRUE(test_serialize(T(-1.0), "-1.0", outcome));
    EXPECT_TRUE(test_serialize(T(4.5), "4.5", outcome));

    if (outcome) {
        return testing::AssertionSuccess();
    } else {
        return testing::AssertionFailure();
    }
}

TEST(Serialization, NumericTypes)
{
    EXPECT_TRUE(test_serialize_integer_type<char>());
    EXPECT_TRUE(test_serialize_integer_type<unsigned char>());
    EXPECT_TRUE(test_serialize_integer_type<short>());
    EXPECT_TRUE(test_serialize_integer_type<unsigned short>());
    EXPECT_TRUE(test_serialize_integer_type<int>());
    EXPECT_TRUE(test_serialize_integer_type<unsigned int>());
    EXPECT_TRUE(test_serialize_integer_type<long>());
    EXPECT_TRUE(test_serialize_integer_type<unsigned long>());
    EXPECT_TRUE(test_serialize_integer_type<long long>());
    EXPECT_TRUE(test_serialize_integer_type<unsigned long long>());

    EXPECT_TRUE(test_serialize_integer_type<int8_t>());
    EXPECT_TRUE(test_serialize_integer_type<uint8_t>());
    EXPECT_TRUE(test_serialize_integer_type<int16_t>());
    EXPECT_TRUE(test_serialize_integer_type<uint16_t>());
    EXPECT_TRUE(test_serialize_integer_type<int32_t>());
    EXPECT_TRUE(test_serialize_integer_type<uint32_t>());
    EXPECT_TRUE(test_serialize_integer_type<int64_t>());
    EXPECT_TRUE(test_serialize_integer_type<uint64_t>());

    EXPECT_TRUE(test_serialize_float_type<float>());
    EXPECT_TRUE(test_serialize_float_type<double>());
}

TEST(Serialization, EnumTypes)
{
    EXPECT_TRUE(test_serialize(Side::Left, "1"));
    EXPECT_TRUE(test_serialize(Side::Right, "2"));

    EXPECT_TRUE(test_serialize(Suit::Diamonds, "0"));
    EXPECT_TRUE(test_serialize(Suit::Hearts, "1"));
    EXPECT_TRUE(test_serialize(Suit::Clubs, "2"));
    EXPECT_TRUE(test_serialize(Suit::Spades, "3"));

    EXPECT_TRUE(test_serialize(Esper::Unu, "\"Unu\""));
    EXPECT_TRUE(test_serialize(Esper::Du, "\"Du\""));
    EXPECT_TRUE(test_serialize(Esper::Kvin, "\"Kvin\""));
    EXPECT_TRUE(test_serialize(Esper::Dek, "\"Dek\""));
}

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

TEST(Serialization, Bytes)
{
    EXPECT_TRUE(test_serialize(make_byte_vector(), "\"\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f' ), "\"Zg==\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o' ), "\"Zm8=\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o', 'o' ), "\"Zm9v\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o', 'o', 'b' ), "\"Zm9vYg==\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o', 'o', 'b', 'a' ), "\"Zm9vYmE=\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o', 'o', 'b', 'a', 'r' ), "\"Zm9vYmFy\""));
}

TEST(Serialization, DateTimeTypes)
{
    EXPECT_TRUE(test_serialize(make_datetime(1982, 10, 23, 1, 20, 40), "\"1982-10-23T01:20:40Z\""));
    EXPECT_TRUE(test_serialize(make_datetime(2022, 2, 1, 23, 2, 1), "\"2022-02-01T23:02:01Z\""));

#if __cplusplus >= 202002L
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    EXPECT_TRUE(test_serialize(year_month_day(1982y, October, 23d), "\"1982-10-23\""));
    EXPECT_TRUE(test_serialize(year_month_day(2022y, February, 1d), "\"2022-02-01\""));
#endif
}

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

TEST(Serialization, Pair)
{
    EXPECT_TRUE(test_serialize(std::make_pair(19, std::string("eighty-two")), "[19,\"eighty-two\"]"));
}

TEST(Serialization, Tuple)
{
    EXPECT_TRUE(test_serialize(std::tuple<>(), "[]"));
    EXPECT_TRUE(test_serialize(std::make_tuple(std::string("value")), "[\"value\"]"));
    EXPECT_TRUE(test_serialize(std::make_tuple(19, std::string("eighty-two")), "[19,\"eighty-two\"]"));
    EXPECT_TRUE(test_serialize(std::make_tuple(23, TestValue("string")), "[23,{\"value\":\"string\"}]"));
}

TEST(Serialization, Variant)
{
    using simple_variant_type = std::variant<int, std::string>;
    EXPECT_TRUE(test_serialize(simple_variant_type("value"), "\"value\""));
    EXPECT_TRUE(test_serialize(simple_variant_type(23), "23"));

    using complex_variant_type = std::variant<TestValue, TestNonCopyable>;
    EXPECT_TRUE(test_serialize(complex_variant_type(TestValue("string")), "{\"value\":\"string\"}"));
    EXPECT_TRUE(test_serialize(complex_variant_type(TestNonCopyable("string")), "{\"member\":{\"value\":\"string\"}}"));
}

TEST(Serialization, Vector)
{
    EXPECT_TRUE(test_serialize(std::vector<int>(), "[]"));
    EXPECT_TRUE(test_serialize(std::vector<int>{1, 2, 3}, "[1,2,3]"));
    EXPECT_TRUE(test_serialize(std::vector<std::vector<int>>{ { 1 }, { 1,2 }, { 1,2,3 } }, "[[1],[1,2],[1,2,3]]"));

    std::vector<std::string> val = { "one", "two" };
    EXPECT_TRUE(test_serialize(val, "[\"one\",\"two\"]"));
}

TEST(Serialization, Array)
{
    EXPECT_TRUE(test_serialize(std::array<int, 0>(), "[]"));
    EXPECT_TRUE(test_serialize(std::array<int, 3>{1, 2, 3}, "[1,2,3]"));

    std::array<std::string, 2> val = { "one", "two" };
    EXPECT_TRUE(test_serialize(val, "[\"one\",\"two\"]"));
}

TEST(Serialization, Set)
{
    EXPECT_TRUE(test_serialize(std::set<int>(), "[]"));
    EXPECT_TRUE(test_serialize(std::set<int>{1, 2, 3}, "[1,2,3]"));

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

TEST(Serialization, Object)
{
    TestValue value_object("test string");
    EXPECT_TRUE(test_serialize(value_object, "{\"value\":\"test string\"}"));

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
    o.member_value = "value";
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

TEST(Deserialization, SpecialTypes)
{
    EXPECT_TRUE(test_deserialize("null", nullptr));
}

TEST(Deserialization, BooleanTypes)
{
    EXPECT_TRUE(test_deserialize("true", true));
    EXPECT_TRUE(test_deserialize("false", false));

    EXPECT_TRUE(test_no_deserialize<bool>("0"));
    EXPECT_TRUE(test_no_deserialize<bool>("1"));
    EXPECT_TRUE(test_no_deserialize<bool>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<bool>("[]"));
    EXPECT_TRUE(test_no_deserialize<bool>("{}"));
}

template<typename T>
testing::AssertionResult test_deserialize_integer_type()
{
    bool outcome = true;
    EXPECT_TRUE(test_deserialize("0", T(0), outcome));
    EXPECT_TRUE(test_deserialize("1", T(1), outcome));

    if constexpr (std::is_signed_v<T>) {
        EXPECT_TRUE(test_deserialize("-1", T(-1), outcome));
    }

    EXPECT_TRUE(test_deserialize(std::to_string(std::numeric_limits<T>::min()), std::numeric_limits<T>::min(), outcome));
    EXPECT_TRUE(test_deserialize(std::to_string(std::numeric_limits<T>::max()), std::numeric_limits<T>::max(), outcome));

    EXPECT_TRUE(test_no_deserialize<T>("1.0", outcome));
    EXPECT_TRUE(test_no_deserialize<T>("-1.0", outcome));
    EXPECT_TRUE(test_no_deserialize<T>("true", outcome));
    EXPECT_TRUE(test_no_deserialize<T>("\"string\"", outcome));
    EXPECT_TRUE(test_no_deserialize<T>("[]", outcome));
    EXPECT_TRUE(test_no_deserialize<T>("{}", outcome));

    if (outcome) {
        return testing::AssertionSuccess();
    } else {
        return testing::AssertionFailure();
    }
}

template<typename T>
testing::AssertionResult test_deserialize_float_type()
{
    bool outcome = true;
    EXPECT_TRUE(test_deserialize("0", T(0.0), outcome));
    EXPECT_TRUE(test_deserialize("0.0", T(0.0), outcome));
    EXPECT_TRUE(test_deserialize("1", T(1.0), outcome));
    EXPECT_TRUE(test_deserialize("1.0", T(1.0), outcome));
    EXPECT_TRUE(test_deserialize("-1", T(-1.0), outcome));
    EXPECT_TRUE(test_deserialize("-1.0", T(-1.0), outcome));
    EXPECT_TRUE(test_deserialize("0.125", T(0.125), outcome));
    EXPECT_TRUE(test_deserialize("-4.5", T(-4.5), outcome));
    EXPECT_TRUE(test_deserialize("2.5e8", T(2.5e8), outcome));
    EXPECT_TRUE(test_deserialize("2.5e-8", T(2.5e-8), outcome));
    EXPECT_TRUE(test_deserialize("-2.5e8", T(-2.5e8), outcome));
    EXPECT_TRUE(test_deserialize("2.5e04", T(2.5e4), outcome));
    EXPECT_TRUE(test_deserialize("2.5e-04", T(2.5e-4), outcome));
    EXPECT_TRUE(test_deserialize("-2.5e04", T(-2.5e4), outcome));
    EXPECT_TRUE(test_deserialize("2.500e4", T(2.5e4), outcome));
    EXPECT_TRUE(test_deserialize("2.500e-4", T(2.5e-4), outcome));
    EXPECT_TRUE(test_deserialize("-2.500e4", T(-2.5e4), outcome));
    EXPECT_TRUE(test_deserialize("2.5e16", T(2.5e16), outcome));
    EXPECT_TRUE(test_deserialize("2.5e-16", T(2.5e-16), outcome));
    EXPECT_TRUE(test_deserialize("-2.5e16", T(-2.5e16), outcome));

    EXPECT_TRUE(test_no_deserialize<T>("true", outcome));
    EXPECT_TRUE(test_no_deserialize<T>("\"string\"", outcome));
    EXPECT_TRUE(test_no_deserialize<T>("[]", outcome));
    EXPECT_TRUE(test_no_deserialize<T>("{}", outcome));

    if (outcome) {
        return testing::AssertionSuccess();
    } else {
        return testing::AssertionFailure();
    }
}

TEST(Deserialization, NumericTypes)
{
    EXPECT_TRUE(test_deserialize_integer_type<char>());
    EXPECT_TRUE(test_deserialize_integer_type<signed char>());
    EXPECT_TRUE(test_deserialize_integer_type<unsigned char>());
    EXPECT_TRUE(test_deserialize_integer_type<short>());
    EXPECT_TRUE(test_deserialize_integer_type<unsigned short>());
    EXPECT_TRUE(test_deserialize_integer_type<int>());
    EXPECT_TRUE(test_deserialize_integer_type<unsigned int>());
    EXPECT_TRUE(test_deserialize_integer_type<long>());
    EXPECT_TRUE(test_deserialize_integer_type<unsigned long>());
    EXPECT_TRUE(test_deserialize_integer_type<long long>());
    EXPECT_TRUE(test_deserialize_integer_type<unsigned long long>());

    EXPECT_TRUE(test_deserialize_float_type<float>());
    EXPECT_TRUE(test_deserialize_float_type<double>());
}

TEST(Deserialization, EnumTypes)
{
    EXPECT_TRUE(test_deserialize("1", Side::Left));
    EXPECT_TRUE(test_deserialize("2", Side::Right));
    EXPECT_TRUE(test_no_deserialize<Side>("\"Left\""));
    EXPECT_TRUE(test_no_deserialize<Side>("\"Right\""));
    EXPECT_TRUE(test_no_deserialize<Side>("true"));
    EXPECT_TRUE(test_no_deserialize<Side>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<Side>("[]"));
    EXPECT_TRUE(test_no_deserialize<Side>("{}"));

    EXPECT_TRUE(test_deserialize("0", Suit::Diamonds));
    EXPECT_TRUE(test_deserialize("1", Suit::Hearts));
    EXPECT_TRUE(test_deserialize("2", Suit::Clubs));
    EXPECT_TRUE(test_deserialize("3", Suit::Spades));
    EXPECT_TRUE(test_no_deserialize<Suit>("\"Diamonds\""));
    EXPECT_TRUE(test_no_deserialize<Suit>("\"Spades\""));

    EXPECT_TRUE(test_deserialize("\"Unu\"", Esper::Unu));
    EXPECT_TRUE(test_deserialize("\"Du\"", Esper::Du));
    EXPECT_TRUE(test_deserialize("\"Kvin\"", Esper::Kvin));
    EXPECT_TRUE(test_deserialize("\"Dek\"", Esper::Dek));
    EXPECT_TRUE(test_no_deserialize<Esper>("0"));
    EXPECT_TRUE(test_no_deserialize<Esper>("1"));
    EXPECT_TRUE(test_no_deserialize<Esper>("\"\""));
    EXPECT_TRUE(test_no_deserialize<Esper>("\"NaN\""));
}

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

TEST(Deserialization, Bytes)
{
    EXPECT_TRUE(test_deserialize("\"\"", make_byte_vector()));
    EXPECT_TRUE(test_deserialize("\"Zg==\"", make_byte_vector('f')));
    EXPECT_TRUE(test_deserialize("\"Zm8=\"", make_byte_vector('f', 'o')));
    EXPECT_TRUE(test_deserialize("\"Zm9v\"", make_byte_vector('f', 'o', 'o')));
    EXPECT_TRUE(test_deserialize("\"Zm9vYg==\"", make_byte_vector('f', 'o', 'o', 'b')));
    EXPECT_TRUE(test_deserialize("\"Zm9vYmE=\"", make_byte_vector('f', 'o', 'o', 'b', 'a')));
    EXPECT_TRUE(test_deserialize("\"Zm9vYmFy\"", make_byte_vector('f', 'o', 'o', 'b', 'a', 'r')));

    EXPECT_TRUE(test_no_deserialize<byte_vector>("\"A\""));
    EXPECT_TRUE(test_no_deserialize<byte_vector>("\"AA\""));
    EXPECT_TRUE(test_no_deserialize<byte_vector>("\"AAA\""));
    EXPECT_TRUE(test_no_deserialize<byte_vector>("true"));
    EXPECT_TRUE(test_no_deserialize<byte_vector>("23"));
    EXPECT_TRUE(test_no_deserialize<byte_vector>("[]"));
    EXPECT_TRUE(test_no_deserialize<byte_vector>("[\"Z\",\"m\",\"9\",\"v\"]"));
    EXPECT_TRUE(test_no_deserialize<byte_vector>("{}"));
}

TEST(Deserialization, DateTimeTypes)
{
    EXPECT_TRUE(test_deserialize("\"1982-10-23T01:20:40Z\"", make_datetime(1982, 10, 23, 1, 20, 40)));
    EXPECT_TRUE(test_deserialize("\"2022-02-01T23:02:01Z\"", make_datetime(2022, 2, 1, 23, 2, 1)));
    EXPECT_TRUE(test_deserialize("\"2000-02-29T00:00:00Z\"", make_datetime(2000, 2, 29, 0, 0, 0)));
    EXPECT_TRUE(test_deserialize("\"2000-03-31T00:00:00Z\"", make_datetime(2000, 3, 31, 0, 0, 0)));

    // space instead of 'T'
    EXPECT_TRUE(test_deserialize("\"2022-02-01 23:02:01Z\"", make_datetime(2022, 2, 1, 23, 2, 1)));

    // two-digit year
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"22-02-01T23:02:01Z\""));

    // missing or wrong time zone
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:02:01\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:02:01A\""));

    // invalid year, month or day
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"YYYY-02-01T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-MM-01T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-00-01T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-99-01T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-DDT23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-00T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-30T23:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-99T23:02:01Z\""));

    // invalid hour, minute or second
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01THH:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T99:02:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:MM:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:99:01Z\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:02:SSZ\""));
    EXPECT_TRUE(test_no_deserialize<timestamp>("\"2022-02-01T23:02:99Z\""));

    // invalid type
    EXPECT_TRUE(test_no_deserialize<timestamp>("true"));
    EXPECT_TRUE(test_no_deserialize<timestamp>("23"));
    EXPECT_TRUE(test_no_deserialize<timestamp>("[]"));
    EXPECT_TRUE(test_no_deserialize<timestamp>("{}"));

#if __cplusplus >= 202002L
    using namespace std::chrono;
    using namespace std::literals::chrono_literals;
    EXPECT_TRUE(test_deserialize("\"1982-10-23\"", year_month_day(1982y, October, 23d)));
    EXPECT_TRUE(test_deserialize("\"2022-02-01\"", year_month_day(2022y, February, 1d)));
    EXPECT_TRUE(test_deserialize("\"2000-02-29\"", year_month_day(2000y, February, 29d)));
    EXPECT_TRUE(test_deserialize("\"2000-03-31\"", year_month_day(2000y, March, 31d)));

    // two-digit year
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"22-02-01\""));

    // invalid year, month or day
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"YYYY-02-01\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-MM-01\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-00-01\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-99-01\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-02-DD\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-02-00\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-02-30\""));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("\"2022-02-99\""));

    // invalid type
    EXPECT_TRUE(test_no_deserialize<year_month_day>("true"));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("23"));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("[]"));
    EXPECT_TRUE(test_no_deserialize<year_month_day>("{}"));
#endif
}

TEST(Deserialization, Pointer)
{
    EXPECT_TRUE(test_deserialize("23", std::make_unique<int>(23)));
    EXPECT_TRUE(test_deserialize("23", std::make_shared<int>(23)));
    EXPECT_TRUE(test_deserialize("\"string\"", std::make_unique<std::string>("string")));
    EXPECT_TRUE(test_deserialize("\"string\"", std::make_shared<std::string>("string")));
    EXPECT_TRUE(test_deserialize("{\"value\": \"string\"}", std::make_unique<TestValue>("string")));
    EXPECT_TRUE(test_deserialize("{\"value\": \"string\"}", std::make_shared<TestValue>("string")));
}

TEST(Deserialization, Pair)
{
    EXPECT_TRUE(test_deserialize("[19, \"eighty-two\"]", std::make_pair(19, std::string("eighty-two"))));

    using pair_type = std::pair<int, std::string>;
    EXPECT_TRUE(test_no_deserialize<pair_type>("[1]"));
    EXPECT_TRUE(test_no_deserialize<pair_type>("[1,\"string1\",23,\"string2\"]"));
    EXPECT_TRUE(test_no_deserialize<pair_type>("true"));
    EXPECT_TRUE(test_no_deserialize<pair_type>("23"));
    EXPECT_TRUE(test_no_deserialize<pair_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<pair_type>("{}"));
}

TEST(Deserialization, Tuple)
{
    EXPECT_TRUE(test_deserialize("[]", std::tuple<>()));
    EXPECT_TRUE(test_deserialize("[\"value\"]", std::make_tuple(std::string("value"))));
    EXPECT_TRUE(test_deserialize("[19, \"eighty-two\"]", std::make_tuple(19, std::string("eighty-two"))));
    EXPECT_TRUE(test_deserialize("[23, {\"value\": \"string\"}]", std::make_tuple(23, TestValue("string"))));

    using tuple_type = std::tuple<int, std::string>;
    EXPECT_TRUE(test_no_deserialize<tuple_type>("[1]"));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("[1,\"string1\",23,\"string2\"]"));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("true"));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("23"));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<tuple_type>("{}"));
}

TEST(Deserialization, Variant)
{
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

    using complex_variant_type = std::variant<TestValue, TestNonCopyable>;
    EXPECT_EQ(deserialize<complex_variant_type>("{\"value\": \"string\"}"), complex_variant_type(TestValue("string")));
    EXPECT_EQ(deserialize<complex_variant_type>("{\"member\":{\"value\": \"string\"}}"), complex_variant_type(TestNonCopyable("string")));

}

TEST(Deserialization, Vector)
{
    EXPECT_TRUE(test_deserialize("[]", std::vector<int>()));
    EXPECT_TRUE(test_deserialize("[1, 2, 3]", std::vector<int> { 1, 2, 3 }));
    EXPECT_TRUE(test_deserialize("[\"one\", \"two\"]", std::vector<std::string> { "one", "two" }));

    std::vector<std::vector<int>> ref_nested = { { 1 }, { 1,2 }, { 1,2,3 } };
    EXPECT_TRUE(test_deserialize("[[1], [1, 2], [1, 2, 3]]", ref_nested));

    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("[\"one\"]"));
    EXPECT_TRUE(test_no_deserialize<std::vector<std::string>>("[2,3]"));
    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("true"));
    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("23"));
    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<std::vector<int>>("{}"));
}

TEST(Deserialization, Array)
{
    EXPECT_TRUE(test_deserialize("[]", std::array<int, 0> {}));
    EXPECT_TRUE(test_deserialize("[1, 2, 3]", std::array<int, 3> { 1, 2, 3 }));
    EXPECT_TRUE(test_deserialize("[\"one\", \"two\"]", std::array<std::string, 2> { "one", "two" }));

    using int_array_type = std::array<int, 2>;
    using string_array_type = std::array<std::string, 2>;
    EXPECT_TRUE(test_no_deserialize<int_array_type>("[]"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("[1]"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("[1,2,3]"));
    EXPECT_TRUE(test_no_deserialize<string_array_type>("[1,2]"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("[\"one\",\"two\"]"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("true"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("23"));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<int_array_type>("{}"));
}

TEST(Deserialization, Set)
{
    EXPECT_TRUE(test_deserialize("[]", std::set<int>()));
    EXPECT_TRUE(test_deserialize("[]", std::set<std::string>()));

    EXPECT_TRUE(test_deserialize(
        "[1, 2, 3]",
        std::set<int> { 1, 2, 3 }
    ));
    EXPECT_TRUE(test_deserialize(
        "[\"one\", \"two\"]",
        std::set<std::string> { "one", "two" }
    ));

    EXPECT_TRUE(test_no_deserialize<std::set<int>>("[\"one\"]"));
    EXPECT_TRUE(test_no_deserialize<std::set<std::string>>("[2,3]"));
    EXPECT_TRUE(test_no_deserialize<std::set<int>>("true"));
    EXPECT_TRUE(test_no_deserialize<std::set<int>>("23"));
    EXPECT_TRUE(test_no_deserialize<std::set<int>>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<std::set<int>>("{}"));
}

TEST(Deserialization, Map)
{
    using map_type = std::map<std::string, int>;
    using string_map_type = std::map<std::string, std::string>;

    EXPECT_TRUE(test_deserialize("{}", map_type()));
    EXPECT_TRUE(test_deserialize("{}", string_map_type()));

    EXPECT_TRUE(test_deserialize(
        "{\"key1\": 1, \"key2\": 2, \"key3\": 3}",
        map_type {
            { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
        }
    ));
    EXPECT_TRUE(test_deserialize(
        "{\"key1\": \"value1\", \"key2\": \"value2\"}",
        string_map_type {
            { "key1", "value1" }, { "key2", "value2" }
        }
    ));

    EXPECT_TRUE(test_no_deserialize<map_type>("true"));
    EXPECT_TRUE(test_no_deserialize<map_type>("23"));
    EXPECT_TRUE(test_no_deserialize<map_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<map_type>("[]"));
    EXPECT_TRUE(test_no_deserialize<map_type>("[1]"));
}

TEST(Deserialization, UnorderedMap)
{
    using map_type = std::unordered_map<std::string, int>;
    using string_map_type = std::unordered_map<std::string, std::string>;

    EXPECT_TRUE(test_deserialize("{}", map_type()));
    EXPECT_TRUE(test_deserialize("{}", string_map_type()));

    EXPECT_TRUE(test_deserialize(
        "{\"key1\": 1, \"key2\": 2, \"key3\": 3}",
        map_type {
            { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
        }
    ));
    EXPECT_TRUE(test_deserialize(
        "{\"key1\": \"value1\", \"key2\": \"value2\"}",
        string_map_type {
            { "key1", "value1" }, { "key2", "value2" }
        }
    ));

    EXPECT_TRUE(test_no_deserialize<map_type>("true"));
    EXPECT_TRUE(test_no_deserialize<map_type>("23"));
    EXPECT_TRUE(test_no_deserialize<map_type>("\"string\""));
    EXPECT_TRUE(test_no_deserialize<map_type>("[]"));
    EXPECT_TRUE(test_no_deserialize<map_type>("[1]"));
}

TEST(Deserialization, Object)
{
    EXPECT_TRUE(test_deserialize("{\"value\":\"test string\"}", TestValue("test string")));
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
    o.member_value = "value";
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

    auto obj = deserialize<TestBackReferenceObject>(json);
    EXPECT_EQ(obj.outer, obj.inner[0]);
    EXPECT_EQ(obj.outer, obj.inner[1]);

    // back-references are not supported by the parser
    EXPECT_THROW(parse<TestBackReferenceObject>(json), JsonParseError);
}

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

    auto&& json = measure("write object to string", [&]() {
        return write_to_string(items);
    });
    std::cout << "JSON string has size of " << json.size() << " B" << std::endl;
    measure("serialize object to DOM", [&]() {
        rapidjson::Document doc;
        serialize_to_document(items, doc);
    });
    measure("deserialize DOM from string", [&]() {
        rapidjson::Document doc;
        doc.Parse(json.data());
    });
    measure("deserialize object from string via DOM with exceptions enabled", [&]() {
        deserialize<std::vector<TestDataTransferObject>>(json);
    });
    measure("deserialize object from string via DOM with exceptions disabled", [&]() {
        std::vector<TestDataTransferObject> obj;
        deserialize(json, obj);
    });
    auto&& values = measure("parse object from string", [&]() {
        return parse<std::vector<TestDataTransferObject>>(json);
    });
    EXPECT_GT(values.size(), 0);
}
#endif

TEST(Documentation, Example)
{
    Example obj = { true, "string", {"a","b","c"}, std::nullopt, {"xyz"} };
    auto json = write_to_string(obj);
    Example res = parse<Example>(json);

    EXPECT_EQ(write_to_string(Derived()), "{\"base_member\":\"base\",\"derived_member\":\"derived\"}");
}

TEST(Schema, Schema)
{
    EXPECT_EQ(schema_to_string<std::nullptr_t>(), "{\"type\":\"null\"}");
    EXPECT_EQ(schema_to_string<bool>(), "{\"type\":\"boolean\"}");
    EXPECT_EQ(schema_to_string<short>(), "{\"type\":\"integer\",\"minimum\":" + std::to_string(std::numeric_limits<short>::min()) + ",\"maximum\":" + std::to_string(std::numeric_limits<short>::max()) + "}");
    EXPECT_EQ(schema_to_string<unsigned short>(), "{\"type\":\"integer\",\"minimum\":" + std::to_string(std::numeric_limits<unsigned short>::min()) + ",\"maximum\":" + std::to_string(std::numeric_limits<unsigned short>::max()) + "}");
    EXPECT_EQ(schema_to_string<int>(), "{\"type\":\"integer\",\"minimum\":" + std::to_string(std::numeric_limits<int>::min()) + ",\"maximum\":" + std::to_string(std::numeric_limits<int>::max()) + "}");
    EXPECT_EQ(schema_to_string<unsigned int>(), "{\"type\":\"integer\",\"minimum\":" + std::to_string(std::numeric_limits<unsigned int>::min()) + ",\"maximum\":" + std::to_string(std::numeric_limits<unsigned int>::max()) + "}");
    EXPECT_EQ(schema_to_string<long>(), "{\"type\":\"integer\",\"minimum\":" + std::to_string(std::numeric_limits<long>::min()) + ",\"maximum\":" + std::to_string(std::numeric_limits<long>::max()) + "}");
    EXPECT_EQ(schema_to_string<unsigned long>(), "{\"type\":\"integer\",\"minimum\":" + std::to_string(std::numeric_limits<unsigned long>::min()) + ",\"maximum\":" + std::to_string(std::numeric_limits<unsigned long>::max()) + "}");
    EXPECT_EQ(schema_to_string<long long>(), "{\"type\":\"integer\",\"minimum\":" + std::to_string(std::numeric_limits<long long>::min()) + ",\"maximum\":" + std::to_string(std::numeric_limits<long long>::max()) + "}");
    EXPECT_EQ(schema_to_string<unsigned long long>(), "{\"type\":\"integer\",\"minimum\":" + std::to_string(std::numeric_limits<unsigned long long>::min()) + ",\"maximum\":" + std::to_string(std::numeric_limits<unsigned long long>::max()) + "}");
    EXPECT_EQ(schema_to_string<float>(), "{\"type\":\"number\"}");
    EXPECT_EQ(schema_to_string<double>(), "{\"type\":\"number\"}");
    EXPECT_EQ(schema_to_string<std::string>(), "{\"type\":\"string\"}");
    EXPECT_EQ(schema_to_string<timestamp>(), "{\"type\":\"string\",\"format\":\"date-time\"}");
#if __cplusplus >= 202002L
    EXPECT_EQ(schema_to_string<std::chrono::year_month_day>(), "{\"type\":\"string\",\"format\":\"date\"}");
#endif

    using array_type = std::array<std::string, 23>;
    EXPECT_EQ(schema_to_string<array_type>(), "{\"type\":\"array\",\"items\":{\"type\":\"string\"},\"minItems\":23,\"maxItems\":23}");
    using pair_type = std::pair<int, std::string>;
    EXPECT_EQ(schema_to_string<pair_type>(), "{\"type\":\"array\",\"prefixItems\":[" + schema_to_string<int>() + "," + schema_to_string<std::string>() + "],\"minItems\":2,\"maxItems\":2}");
    using tuple_type = std::tuple<int, double, std::string>;
    EXPECT_EQ(schema_to_string<tuple_type>(), "{\"type\":\"array\",\"prefixItems\":[" + schema_to_string<int>() + "," + schema_to_string<double>() + "," + schema_to_string<std::string>() + "],\"minItems\":3,\"maxItems\":3}");
    using variant_type = std::variant<int, double, std::string>;
    EXPECT_EQ(schema_to_string<variant_type>(), "{\"oneOf\":[" + schema_to_string<int>() + "," + schema_to_string<double>() + "," + schema_to_string<std::string>() + "]}");

    EXPECT_EQ(schema_to_string<std::vector<std::string>>(), "{\"type\":\"array\",\"items\":{\"type\":\"string\"}}");
    EXPECT_EQ(schema_to_string<std::set<std::string>>(), "{\"type\":\"array\",\"items\":{\"type\":\"string\"},\"uniqueItems\":true}");

    using map_type = std::map<std::string, double>;
    EXPECT_EQ(schema_to_string<map_type>(), "{\"type\":\"object\",\"additionalProperties\":{\"type\":\"number\"}}");
    using unordered_map_type = std::unordered_map<std::string, double>;
    EXPECT_EQ(schema_to_string<unordered_map_type>(), "{\"type\":\"object\",\"additionalProperties\":{\"type\":\"number\"}}");

    EXPECT_EQ(
        schema_to_string<TestValue>(),
        "{\"type\":\"object\",\"properties\":{\"value\":{\"type\":\"string\"}},\"required\":[\"value\"],\"additionalProperties\":false}"
    );
    EXPECT_EQ(
        schema_to_string<TestOptionalObjectMember>(),
        "{\"type\":\"object\",\"properties\":{\"optional_value\":" + schema_to_string<int>() + "},\"required\":[],\"additionalProperties\":false}"
    );
    EXPECT_EQ(
        schema_to_string<TestDerived>(),
        "{\"type\":\"object\",\"properties\":{\"value\":{\"type\":\"string\"},\"member\":{\"type\":\"string\"}},\"required\":[\"value\",\"member\"],\"additionalProperties\":false}"
    );
    EXPECT_EQ(
        schema_to_string<std::vector<TestValue>>(),
        "{\"definitions\":{\"" + std::string(typeid(TestValue).name()) + "\":" + schema_to_string<TestValue>() + "},\"type\":\"array\",\"items\":{\"$ref\":\"#/definitions/" + std::string(typeid(TestValue).name()) + "\"}}"
    );
    EXPECT_EQ(schema_to_string<std::unique_ptr<TestValue>>(), schema_to_string<TestValue>());
    EXPECT_EQ(schema_to_string<std::shared_ptr<TestValue>>(), schema_to_string<TestValue>());

    EXPECT_EQ(
        schema_document_to_string<bool>(),
        "{\"$schema\":\"https://json-schema.org/draft/2020-12/schema\",\"type\":\"boolean\"}"
    );
    EXPECT_EQ(
        schema_document_to_string<TestValue>(),
        "{\"$schema\":\"https://json-schema.org/draft/2020-12/schema\",\"type\":\"object\",\"properties\":{\"value\":{\"type\":\"string\"}},\"required\":[\"value\"],\"additionalProperties\":false}"
    );
}
