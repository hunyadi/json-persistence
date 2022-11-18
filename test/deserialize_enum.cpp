#include <gtest/gtest.h>
#include "example_enums.hpp"
#include "example_enum_traits.hpp"
#include "persistence/parse_enum.hpp"
#include "persistence/parse.hpp"
#include "persistence/deserialize_enum.hpp"
#include "persistence/deserialize.hpp"
#include "test_deserialize.hpp"

using namespace test;

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
