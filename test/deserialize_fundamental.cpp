#include <gtest/gtest.h>
#include "persistence/parse_fundamental.hpp"
#include "persistence/parse.hpp"
#include "persistence/deserialize_fundamental.hpp"
#include "persistence/deserialize.hpp"
#include "test_deserialize.hpp"

using namespace test;

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
