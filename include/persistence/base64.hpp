#pragma once
#include "bytes.hpp"
#include "detail/unlikely.hpp"
#include <array>

namespace persistence
{
    namespace detail
    {
        enum class endianness
        {
        #ifdef _WIN32
            little = 0,
            big    = 1,
            native = little
        #else
            little = __ORDER_LITTLE_ENDIAN__,
            big    = __ORDER_BIG_ENDIAN__,
            native = __BYTE_ORDER__
        #endif
        };

        constexpr std::array<char, 64> encode_table {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
        };

        /**
         * Produces the Base64-encoding of a sequence of bytes.
         *
         * @param in_a First byte to encode.
         * @param in_b Second byte to encode.
         * @param in_c Third byte to encode.
         * @param out A pointer to a string to receive 4 Base64-encoded characters.
         */
        inline void encode_triplet(std::byte in_a, std::byte in_b, std::byte in_c, char* out)
        {
            auto a = std::to_integer<std::size_t>(in_a);
            auto b = std::to_integer<std::size_t>(in_b);
            auto c = std::to_integer<std::size_t>(in_c);

            *out++ = encode_table[a >> 2];
            *out++ = encode_table[((a & 0b00000011) << 4) | (b >> 4)];
            *out++ = encode_table[((b & 0b00001111) << 2) | (c >> 6)];
            *out++ = encode_table[c & 0b00111111];
        }

        /**
         * Produces the Base64-encoding of the first 6 bytes of a 64-bit integer.
         *
         * @param value An integer with big-endian representation.
         * @param out A pointer to a string to receive 8 Base64-encoded characters.
         */
        inline void encode_sextet_big_endian(std::uint64_t value, char* out)
        {
            *out++ = encode_table[(value >> 58) & 0x3F];
            *out++ = encode_table[(value >> 52) & 0x3F];
            *out++ = encode_table[(value >> 46) & 0x3F];
            *out++ = encode_table[(value >> 40) & 0x3F];
            *out++ = encode_table[(value >> 34) & 0x3F];
            *out++ = encode_table[(value >> 28) & 0x3F];
            *out++ = encode_table[(value >> 22) & 0x3F];
            *out++ = encode_table[(value >> 16) & 0x3F];
        }

        /**
         * Produces the Base64-encoding of the first 6 bytes of a 64-bit integer.
         *
         * @param value An integer with little-endian representation.
         * @param out A pointer to a string to receive 8 Base64-encoded characters.
         */
        inline void encode_sextet_little_endian(std::uint64_t value, char* out)
        {
            *out++ = encode_table[(value >> 2) & 0b00111111];
            *out++ = encode_table[((value & 0b00000011) << 4) | ((value >> 12) & 0b00001111)];
            *out++ = encode_table[(((value >> 8) & 0b00001111) << 2) | ((value >> 22) & 0b00000011)];
            *out++ = encode_table[(value >> 16) & 0b00111111];
            *out++ = encode_table[(value >> 26) & 0b00111111];
            *out++ = encode_table[(((value >> 24) & 0b00000011) << 4) | ((value >> 36) & 0b00001111)];
            *out++ = encode_table[(((value >> 32) & 0b00001111) << 2) | ((value >> 46) & 0b00000011)];
            *out++ = encode_table[(value >> 40) & 0b00111111];
        }
    }

    /**
     * Generates the Base64-encoded representation of a raw sequence of bytes.
     *
     * @param input A sequence of bytes.
     * @param output A Base64-encoded string.
     * @return True on success.
     */
    inline bool base64_encode(const byte_view& input, std::string& output)
    {
        using detail::encode_triplet;

        output.clear();
        if (input.size() == 0) {
            return true;
        }

        std::size_t fast_sextets = input.size() / 24 * 4;  // round to multiples of 8 bytes to ensure pointer stays in addressable range
        std::size_t full_triplets = input.size() / 3;
        std::size_t remaining_triplets = full_triplets - fast_sextets * 2;
        std::size_t remaining_bytes = input.size() % 3;
        std::size_t output_size = full_triplets * 4 + (remaining_bytes > 0 ? 4 : 0);
        output.resize(output_size);

        const std::byte* p = input.data();
        char* r = output.data();

        // iterate over data with 64-bit words, processing full sextets
        for (std::size_t i = 0; i < fast_sextets; ++i) {
            uint64_t value = *reinterpret_cast<const uint64_t*>(p);
            if constexpr (detail::endianness::native == detail::endianness::little) {
                detail::encode_sextet_little_endian(value, r);
            } else if constexpr (detail::endianness::native == detail::endianness::big) {
                detail::encode_sextet_big_endian(value, r);
            }
            p += 6;
            r += 8;
        }

        // process remaining full triplets (if any)
        for (std::size_t i = 0; i < remaining_triplets; ++i) {
            std::byte a = *p++;
            std::byte b = *p++;
            std::byte c = *p++;
            encode_triplet(a, b, c, r);
            r += 4;
        }

        // process remaining bytes (last incomplete triplet)
        if (remaining_bytes == 2) {
            std::byte a = input[input.size() - 2];
            std::byte b = input[input.size() - 1];
            encode_triplet(a, b, std::byte{0x00}, r);
            r += 3;
            *r++ = '=';
        } else if (remaining_bytes == 1) {
            std::byte a = input.back();
            encode_triplet(a, std::byte{0x00}, std::byte{0x00}, r);
            r += 2;
            *r++ = '=';
            *r++ = '=';
        }

        return true;
    }

    /**
     * Generates the Base64-encoded representation of a raw sequence of bytes.
     *
     * @param input A sequence of bytes.
     * @param output A Base64-encoded string.
     * @return True on success.
     */
    inline bool base64_encode(const byte_vector& input, std::string& output)
    {
        return base64_encode(
            byte_view(
                input.data(),
                input.size()
            ),
            output
        );
    }

    /**
     * Generates the Base64-encoded representation of a string.
     *
     * @param input An input string.
     * @param output A Base64-encoded string.
     * @return True on success.
     */
    inline bool base64_encode(const std::string_view& input, std::string& output)
    {
        return base64_encode(
            byte_view(
                reinterpret_cast<const std::byte*>(input.data()),
                input.size()
            ),
            output
        );
    }

    namespace detail
    {
        constexpr std::array<std::uint8_t, 256> decode_table {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
            0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
            0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
            0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };

        /**
         * Produces the Base64-decoding of a sequence of characters.
         *
         * @param in_a First byte to decode.
         * @param in_b Second byte to decode.
         * @param in_c Third byte to decode.
         * @param in_d Fourth byte to decode.
         * @param out A pointer to a buffer to receive 3 Base64-decoded bytes.
         */
        inline bool decode_quadruplet(char in_a, char in_b, char in_c, char in_d, std::byte* out)
        {
            auto out_a = decode_table[in_a];
            auto out_b = decode_table[in_b];
            auto out_c = decode_table[in_c];
            auto out_d = decode_table[in_d];

            PERSISTENCE_IF_UNLIKELY(((out_a | out_b | out_c | out_d) & 0b11000000) != 0) {
                return false;
            }

            *out++ = std::byte((out_a << 2) | (out_b >> 4));
            *out++ = std::byte(((out_b & 0b00001111) << 4) | (out_c >> 2));
            *out++ = std::byte(((out_c & 0b00000011) << 6) | out_d);
            return true;
        }
    }

    /**
     * Parses the Base64-encoded representation of a sequence of bytes.
     *
     * @param input A Base64-encoded string.
     * @param output A sequence of bytes.
     * @return True if parsing is successful, false if input is invalid.
     */
    inline bool base64_decode(const std::string_view& input, byte_vector& output)
    {
        using detail::decode_quadruplet;

        output.clear();
        if (input.size() == 0) {
            return true;
        }

        if ((input.size() % 4) != 0) {
            return false;
        }

        std::size_t quadruplets = input.size() / 4;
        output.resize(quadruplets * 3);

        const char* p = input.data();
        std::byte* r = output.data();

        for (std::size_t i = 1; i < quadruplets; ++i) {
            char a = *p++;
            char b = *p++;
            char c = *p++;
            char d = *p++;

            // process all but last quadruplet
            bool result = decode_quadruplet(a, b, c, d, r);
            if (!result) {
                return false;
            }

            r += 3;
        }

        // process last quadruplet
        if (input[input.size() - 2] == '=') {
            if (input[input.size() - 1] != '=') {
                return false;
            }

            bool result = decode_quadruplet(
                input[input.size() - 4],
                input[input.size() - 3],
                'A',
                'A',
                r
            );
            if (!result) {
                return false;
            }
            output.pop_back();
            output.pop_back();
        } else if (input[input.size() - 1] == '=') {
            bool result = decode_quadruplet(
                input[input.size() - 4],
                input[input.size() - 3],
                input[input.size() - 2],
                'A',
                r
            );
            if (!result) {
                return false;
            }
            output.pop_back();
        } else {
            bool result = decode_quadruplet(
                input[input.size() - 4],
                input[input.size() - 3],
                input[input.size() - 2],
                input[input.size() - 1],
                r
            );
            if (!result) {
                return false;
            }
        }

        return true;
    }
}
