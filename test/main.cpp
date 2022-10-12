#include <gtest/gtest.h>
#include "definitions.hpp"
#include "persistence/persistence.hpp"

using namespace persistence;

template<typename T>
void test_integer_type()
{
    EXPECT_EQ(serialize_to_string(T(0)), "0");
    EXPECT_EQ(serialize_to_string(T(1)), "1");
    EXPECT_EQ(serialize_to_string(std::numeric_limits<T>::min()), std::to_string(std::numeric_limits<T>::min()));
    EXPECT_EQ(serialize_to_string(std::numeric_limits<T>::max()), std::to_string(std::numeric_limits<T>::max()));
}

template<typename T>
void test_float_type()
{
    EXPECT_EQ(serialize_to_string(T(0)), "0.0");
    EXPECT_EQ(serialize_to_string(T(1)), "1.0");
    EXPECT_EQ(serialize_to_string(T(4.5)), "4.5");
}

TEST(Serialization, SpecialTypes)
{
    EXPECT_EQ(serialize_to_string(nullptr), "null");
}

TEST(Serialization, BooleanTypes)
{
    EXPECT_EQ(serialize_to_string(true), "true");
    EXPECT_EQ(serialize_to_string(false), "false");
}

TEST(Serialization, NumericTypes)
{
    test_integer_type<char>();
    test_integer_type<unsigned char>();
    test_integer_type<int>();
    test_integer_type<unsigned int>();
    test_integer_type<long>();
    test_integer_type<unsigned long>();
    test_integer_type<long long>();
    test_integer_type<unsigned long long>();

    test_float_type<float>();
    test_float_type<double>();
}

TEST(Serialization, EnumTypes)
{
    EXPECT_EQ(serialize_to_string(Side::Left), "1");
    EXPECT_EQ(serialize_to_string(Side::Right), "2");

    EXPECT_EQ(serialize_to_string(Suit::Diamonds), "0");
    EXPECT_EQ(serialize_to_string(Suit::Hearts), "1");
    EXPECT_EQ(serialize_to_string(Suit::Clubs), "2");
    EXPECT_EQ(serialize_to_string(Suit::Spades), "3");
}

TEST(Serialization, StringTypes)
{
    EXPECT_EQ(serialize_to_string(""), "\"\"");
    EXPECT_EQ(serialize_to_string("test string"), "\"test string\"");

    EXPECT_EQ(serialize_to_string(std::string_view()), "\"\"");
    EXPECT_EQ(serialize_to_string(std::string_view("test string")), "\"test string\"");

    EXPECT_EQ(serialize_to_string(std::string()), "\"\"");
    EXPECT_EQ(serialize_to_string(std::string("test string")), "\"test string\"");
}

TEST(Serialization, DateTimeTypes)
{
    EXPECT_EQ(
        serialize_to_string(make_datetime(1982, 10, 23, 1, 20, 40)),
        "\"1982-10-23T01:20:40Z\""
    );
    EXPECT_EQ(
        serialize_to_string(make_datetime(2022, 2, 1, 23, 2, 1)),
        "\"2022-02-01T23:02:01Z\""
    );
}

TEST(Serialization, Pair)
{
    EXPECT_EQ(
        serialize_to_string(std::make_pair(19, std::string("eighty-two"))),
        "[19,\"eighty-two\"]"
    );
}

TEST(Serialization, Tuple)
{
    EXPECT_EQ(serialize_to_string(std::tuple<>()), "[]");
    EXPECT_EQ(
        serialize_to_string(std::make_tuple(std::string("value"))),
        "[\"value\"]"
    );
    EXPECT_EQ(
        serialize_to_string(std::make_tuple(19, std::string("eighty-two"))),
        "[19,\"eighty-two\"]"
    );
}

TEST(Serialization, Variant)
{
    using variant_type = std::variant<int, std::string>;
    EXPECT_EQ(serialize_to_string(variant_type("value")), "\"value\"");
    EXPECT_EQ(serialize_to_string(variant_type(23)), "23");
}

TEST(Serialization, Vector)
{
    EXPECT_EQ(serialize_to_string(std::vector<int>()), "[]");
    EXPECT_EQ(serialize_to_string(std::vector<int>{1, 2, 3}), "[1,2,3]");

    std::vector<std::string> val = { "one", "two" };
    EXPECT_EQ(serialize_to_string(val), "[\"one\",\"two\"]");
}

TEST(Serialization, Array)
{
    EXPECT_EQ(serialize_to_string(std::array<int, 0>()), "[]");
    EXPECT_EQ(serialize_to_string(std::array<int, 3>{1, 2, 3}), "[1,2,3]");

    std::array<std::string, 2> val = { "one", "two" };
    EXPECT_EQ(serialize_to_string(val), "[\"one\",\"two\"]");
}

TEST(Serialization, Set)
{
    EXPECT_EQ(serialize_to_string(std::set<int>()), "[]");
    EXPECT_EQ(serialize_to_string(std::set<int>{1, 2, 3}), "[1,2,3]");

    std::set<std::string> val = { "one", "two" };
    EXPECT_EQ(serialize_to_string(val), "[\"one\",\"two\"]");
}

TEST(Serialization, Map)
{
    EXPECT_EQ(serialize_to_string(std::map<std::string, int>()), "{}");

    std::map<std::string, int> val = {
        { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
    };
    EXPECT_EQ(serialize_to_string(val), "{\"key1\":1,\"key2\":2,\"key3\":3}");
}

TEST(Serialization, UnorderedMap)
{
    EXPECT_EQ(serialize_to_string(std::unordered_map<std::string, int>()), "{}");

    std::unordered_map<std::string, int> val = {
        { "key", 1 }
    };
    EXPECT_EQ(serialize_to_string(val), "{\"key\":1}");
}

TEST(Serialization, Object)
{
    TestValue value_object("test string");
    EXPECT_EQ(serialize_to_string(value_object), "{\"value\":\"test string\"}");

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
    EXPECT_EQ(serialize_to_string(dto),
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
    EXPECT_EQ(serialize_to_string(opt1), "{}");

    TestOptionalObjectMember opt2;
    opt2.optional_value = 42;
    EXPECT_EQ(serialize_to_string(opt2), "{\"optional_value\":42}");
}

TEST(Deserialization, SpecialTypes)
{
    EXPECT_EQ(deserialize<nullptr_t>("null"), nullptr);
}

TEST(Deserialization, BooleanTypes)
{
    EXPECT_EQ(deserialize<bool>("true"), true);
    EXPECT_EQ(deserialize<bool>("false"), false);

    bool result;
    EXPECT_FALSE(deserialize<bool>("", result));
    EXPECT_FALSE(deserialize<bool>("0", result));
    EXPECT_FALSE(deserialize<bool>("1", result));
}

TEST(Deserialization, NumericTypes)
{
    EXPECT_EQ(deserialize<char>("1"), static_cast<char>(1));
    EXPECT_EQ(deserialize<unsigned char>("2"), static_cast<unsigned char>(2));
    EXPECT_EQ(deserialize<int>("3"), 3);
    EXPECT_EQ(deserialize<unsigned int>("4"), 4u);
    EXPECT_EQ(deserialize<long>("5"), 5l);
    EXPECT_EQ(deserialize<unsigned long>("6"), 6ul);
    EXPECT_EQ(deserialize<long long>("7"), 7ll);
    EXPECT_EQ(deserialize<unsigned long long>("8"), 8ull);

    EXPECT_EQ(deserialize<float>("9.5"), 9.5f);
    EXPECT_EQ(deserialize<double>("1.25"), 1.25);

    int result;
    EXPECT_FALSE(deserialize<int>("", result));
    EXPECT_FALSE(deserialize<int>("1.0", result));
}

TEST(Deserialization, EnumTypes)
{
    EXPECT_EQ(deserialize<Side>("1"), Side::Left);
    EXPECT_EQ(deserialize<Side>("2"), Side::Right);

    EXPECT_EQ(deserialize<Suit>("0"), Suit::Diamonds);
    EXPECT_EQ(deserialize<Suit>("1"), Suit::Hearts);
    EXPECT_EQ(deserialize<Suit>("2"), Suit::Clubs);
    EXPECT_EQ(deserialize<Suit>("3"), Suit::Spades);
}

TEST(Deserialization, StringTypes)
{
    EXPECT_EQ(deserialize<std::string>("\"\""), std::string());
    EXPECT_EQ(deserialize<std::string>("\"test string\""), std::string("test string"));

    std::string result;
    EXPECT_FALSE(deserialize<std::string>("", result));
}

TEST(Deserialization, DateTimeTypes)
{
    EXPECT_EQ(
        deserialize<timestamp>("\"1982-10-23T01:20:40Z\""),
        make_datetime(1982, 10, 23, 1, 20, 40)
    );
    EXPECT_EQ(
        deserialize<timestamp>("\"2022-02-01T23:02:01Z\""),
        make_datetime(2022, 2, 1, 23, 2, 1)
    );

    timestamp result;

    // space instead of 'T'
    EXPECT_TRUE(deserialize<timestamp>("\"2022-02-01 23:02:01Z\"", result));

    // two-digit year
    EXPECT_FALSE(deserialize<timestamp>("\"22-02-01T23:02:01Z\"", result));

    // missing or wrong time zone
    EXPECT_FALSE(deserialize<timestamp>("\"2022-02-01T23:02:01\"", result));
    EXPECT_FALSE(deserialize<timestamp>("\"2022-02-01T23:02:01A\"", result));

    // invalid year, month, day, hour, minute or second
    EXPECT_FALSE(deserialize<timestamp>("\"YYYY-02-01T23:02:01Z\"", result));
    EXPECT_FALSE(deserialize<timestamp>("\"2022-MM-01T23:02:01Z\"", result));
    EXPECT_FALSE(deserialize<timestamp>("\"2022-02-DDT23:02:01Z\"", result));
    EXPECT_FALSE(deserialize<timestamp>("\"2022-02-01THH:02:01Z\"", result));
    EXPECT_FALSE(deserialize<timestamp>("\"2022-02-01T23:MM:01Z\"", result));
    EXPECT_FALSE(deserialize<timestamp>("\"2022-02-01T23:02:SSZ\"", result));
}

TEST(Deserialization, Pair)
{
    auto ref = std::make_pair(19, std::string("eighty-two"));
    auto val = deserialize<std::pair<int, std::string>>("[19, \"eighty-two\"]");
    EXPECT_EQ(val, ref);
}

TEST(Deserialization, Tuple)
{
    EXPECT_EQ(deserialize<std::tuple<>>("[]"), std::tuple<>());

    auto ref_str = std::make_tuple(std::string("value"));
    auto val_str = deserialize<std::tuple<std::string>>("[\"value\"]");
    EXPECT_EQ(val_str, ref_str);

    auto ref_tup = std::make_tuple(19, std::string("eighty-two"));
    auto val_tup = deserialize<std::tuple<int, std::string>>("[19, \"eighty-two\"]");
    EXPECT_EQ(val_tup, ref_tup);
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
    EXPECT_EQ(deserialize<std::vector<int>>("[]"), std::vector<int>());

    std::vector<int> ref_int = { 1, 2, 3 };
    auto val_int = deserialize<std::vector<int>>("[1, 2, 3]");
    EXPECT_EQ(val_int, ref_int);

    std::vector<std::string> ref_str = { "one", "two" };
    auto val_str = deserialize<std::vector<std::string>>("[\"one\", \"two\"]");
    EXPECT_EQ(val_str, ref_str);
}

TEST(Deserialization, Array)
{
    std::array<int, 0> ref_empty;
    auto val_empty = deserialize<std::array<int, 0>>("[]");
    EXPECT_EQ(val_empty, ref_empty);

    std::array<int, 3> ref_int = { 1,2,3 };
    auto val_int = deserialize<std::array<int, 3>>("[1, 2, 3]");
    EXPECT_EQ(val_int, ref_int);
}

TEST(Deserialization, Set)
{
    EXPECT_EQ(deserialize<std::set<int>>("[]"), std::set<int>());

    std::set<int> ref = { 1,2,3 };
    auto val = deserialize<std::set<int>>("[1, 2, 3]");
    EXPECT_EQ(val, ref);
}

TEST(Deserialization, Map)
{
    using ordered_map_type = std::map<std::string, int>;
    ordered_map_type ref = {
        { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
    };
    auto val = deserialize<ordered_map_type>("{\"key1\":1, \"key2\":2, \"key3\":3}");
    EXPECT_EQ(val, ref);
}

TEST(Deserialization, UnorderedMap)
{
    using unordered_map_type = std::unordered_map<std::string, int>;
    unordered_map_type ref = {
        { "key1", 1 }, { "key2", 2 }, { "key3", 3 }
    };
    auto val = deserialize<unordered_map_type>("{\"key1\": 1, \"key2\": 2, \"key3\": 3}");
    EXPECT_EQ(val, ref);
}

TEST(Deserialization, Object)
{
    const char* json =
        "{\"bool_value\":true,"
        "\"bool_list\":[false,false,true],"
        "\"int_value\":42,"
        "\"int_list\":[82,10,23],"
        "\"string_value\":\"test string\","
        "\"string_list\":[\"one\",\"two\",\"three\"],"
        "\"object_value\":{\"member_value\":\"value\"},"
        "\"object_list\":[{\"member_value\":\"value\"},{\"member_value\":\"value\"}]}"
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

    auto val = deserialize<TestDataTransferObject>(json);

    EXPECT_EQ(val.bool_value, ref.bool_value);
    EXPECT_EQ(val.bool_list, ref.bool_list);
    EXPECT_EQ(val.int_value, ref.int_value);
    EXPECT_EQ(val.int_list, ref.int_list);
    EXPECT_EQ(val.string_value, ref.string_value);
    EXPECT_EQ(val.string_list, ref.string_list);
    EXPECT_EQ(val.object_value, ref.object_value);
    EXPECT_EQ(val.object_list, ref.object_list);
}

TEST(Deserialization, Optional)
{
    EXPECT_EQ(deserialize<TestOptionalObjectMember>("{}"), TestOptionalObjectMember());
    EXPECT_EQ(deserialize<TestOptionalObjectMember>("{\"optional_value\": 42}"), TestOptionalObjectMember(42));
}
