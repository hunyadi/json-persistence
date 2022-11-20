#include <gtest/gtest.h>
#ifdef PERSISTENCE_BOOST_DESCRIBE
#include "persistence/write_enum.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_enum.hpp"
#include "persistence/serialize.hpp"
#include "test_serialize.hpp"

#include "persistence/parse_enum.hpp"
#include "persistence/parse.hpp"
#include "persistence/deserialize_enum.hpp"
#include "persistence/deserialize.hpp"
#include "test_deserialize.hpp"

#include <boost/describe.hpp>

namespace
{
    enum class Side : short
    {
        Left = 1,
        Right = 2
    };

    enum class Suit
    {
        Diamonds,
        Hearts,
        Clubs,
        Spades
    };

    BOOST_DESCRIBE_ENUM(Side, Left, Right)
    BOOST_DESCRIBE_ENUM(Suit, Diamonds, Hearts, Clubs, Spades)
}

using namespace test;

TEST(Serialization, BoostEnumTypes)
{
    EXPECT_TRUE(test_serialize(Side::Left, "\"Left\""));
    EXPECT_TRUE(test_serialize(Side::Right, "\"Right\""));

    EXPECT_TRUE(test_serialize(Suit::Diamonds, "\"Diamonds\""));
    EXPECT_TRUE(test_serialize(Suit::Hearts, "\"Hearts\""));
    EXPECT_TRUE(test_serialize(Suit::Clubs, "\"Clubs\""));
    EXPECT_TRUE(test_serialize(Suit::Spades, "\"Spades\""));
}

TEST(Deserialization, BoostEnumTypes)
{
    EXPECT_TRUE(test_deserialize("\"Left\"", Side::Left));
    EXPECT_TRUE(test_deserialize("\"Right\"", Side::Right));
    EXPECT_TRUE(test_no_deserialize<Side>("1"));
    EXPECT_TRUE(test_no_deserialize<Side>("2"));

    EXPECT_TRUE(test_deserialize("\"Diamonds\"", Suit::Diamonds));
    EXPECT_TRUE(test_deserialize("\"Hearts\"", Suit::Hearts));
    EXPECT_TRUE(test_deserialize("\"Clubs\"", Suit::Clubs));
    EXPECT_TRUE(test_deserialize("\"Spades\"", Suit::Spades));
    EXPECT_TRUE(test_no_deserialize<Suit>("0"));
    EXPECT_TRUE(test_no_deserialize<Suit>("3"));
}
#endif
