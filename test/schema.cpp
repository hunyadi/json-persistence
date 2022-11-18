#include <gtest/gtest.h>
#include "persistence/schema_utility.hpp"
#include "example_classes.hpp"

using namespace persistence;

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
