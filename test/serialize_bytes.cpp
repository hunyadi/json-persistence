#include <gtest/gtest.h>
#include "persistence/write_bytes.hpp"
#include "persistence/write.hpp"
#include "persistence/serialize_bytes.hpp"
#include "persistence/serialize.hpp"
#include "test_serialize.hpp"

using namespace test;

TEST(Serialization, Bytes)
{
    using persistence::make_byte_vector;

    EXPECT_TRUE(test_serialize(make_byte_vector(), "\"\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f' ), "\"Zg==\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o' ), "\"Zm8=\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o', 'o' ), "\"Zm9v\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o', 'o', 'b' ), "\"Zm9vYg==\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o', 'o', 'b', 'a' ), "\"Zm9vYmE=\""));
    EXPECT_TRUE(test_serialize(make_byte_vector( 'f', 'o', 'o', 'b', 'a', 'r' ), "\"Zm9vYmFy\""));
}
