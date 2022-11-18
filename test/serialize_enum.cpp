#include <gtest/gtest.h>
#include "example_enums.hpp"
#include "example_enum_traits.hpp"
#include "persistence/write_enum.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_enum.hpp"
#include "persistence/serialize.hpp"
#include "test_serialize.hpp"

using namespace test;

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
