#include <gtest/gtest.h>
#include "persistence/parse_bytes.hpp"
#include "persistence/parse.hpp"
#include "persistence/deserialize_bytes.hpp"
#include "persistence/deserialize.hpp"
#include "test_deserialize.hpp"

using namespace test;

TEST(Deserialization, Bytes)
{
    using persistence::byte_vector;
    using persistence::make_byte_vector;

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
