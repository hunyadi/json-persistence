#include <gtest/gtest.h>
#include "persistence/write_fundamental.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_fundamental.hpp"
#include "persistence/serialize.hpp"
#include "test_serialize.hpp"

using namespace test;

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
