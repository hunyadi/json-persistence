#pragma once
#include "bytes.hpp"
#include "detail/unlikely.hpp"
#include <array>

namespace persistence
{
    namespace detail
    {
        inline bool is_valid_base64_char(char c)
        {
            return ((c >= 'A') && (c <= 'Z'))
                || ((c >= 'a') && (c <= 'z'))
                || ((c >= '0') && (c <= '9'))
                || ((c == '+') || (c == '/'))
            ;
        }

        constexpr std::array<char, 64> encode_table {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
            'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
            'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
        };

        using triplet_type = std::uint32_t;

        /**
        * Produces the Base64-encoding of a sequence of bytes.
        *
        * @param concat_bytes An integer storing data on the 3 least-significant bytes.
        * @param output A string to receive 4 Base64-encoded characters.
        */
        inline void encode_triplet(triplet_type concat_bytes, std::string& output)
        {
            output.push_back(encode_table[(concat_bytes >> 18) & 0x3F]);
            output.push_back(encode_table[(concat_bytes >> 12) & 0x3F]);
            output.push_back(encode_table[(concat_bytes >>  6) & 0x3F]);
            output.push_back(encode_table[concat_bytes & 0x3F]);
        }

        /**
        * Produces the Base64-encoding of a sequence of bytes.
        *
        * @param a First byte to encode.
        * @param b Second byte to encode.
        * @param c Third byte to encode.
        * @param output A string to receive 4 Base64-encoded characters.
        */
        inline void encode_triplet(std::byte a, std::byte b, std::byte c, std::string& output)
        {
            triplet_type concat_bytes =
                (std::to_integer<triplet_type>(a) << 16) |
                (std::to_integer<triplet_type>(b) <<  8) |
                std::to_integer<triplet_type>(c)
            ;
            encode_triplet(concat_bytes, output);
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

        std::size_t full_triplets = input.size() / 3;
        std::size_t remaining_bytes = input.size() % 3;
        std::size_t output_size = full_triplets * 4 + (remaining_bytes > 0 ? 4 : 0);
        output.reserve(output_size);

        const std::byte* p = input.data();

        // process all complete triplets
        for (std::size_t i = 0; i < full_triplets; ++i) {
            std::byte a = *p++;
            std::byte b = *p++;
            std::byte c = *p++;
            encode_triplet(a, b, c, output);
        }

        // process remaining bytes (last incomplete triplet)
        if (remaining_bytes == 2) {
            std::byte a = input[input.size() - 2];
            std::byte b = input[input.size() - 1];
            encode_triplet(a, b, std::byte{0x00}, output);
            output.pop_back();
            output.push_back('=');
        } else if (remaining_bytes == 1) {
            std::byte a = input.back();
            encode_triplet(a, std::byte{0x00}, std::byte{0x00}, output);
            output.pop_back();
            output.pop_back();
            output.push_back('=');
            output.push_back('=');
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

        using quadruplet_type = std::uint32_t;

        /**
        * Produces the Base64-decoding of a sequence of bytes.
        *
        * @param concat_bytes An integer storing data on the 4 least-significant bytes.
        * @param output A vector to receive 3 Base64-decoded bytes.
        */
        inline void decode_quadruplet(quadruplet_type concat_bytes, byte_vector& out)
        {
            out.push_back(std::byte((concat_bytes >> 16) & 0xFF));
            out.push_back(std::byte((concat_bytes >> 8) & 0xFF));
            out.push_back(std::byte(concat_bytes & 0xFF));
        }

        /**
        * Produces the Base64-decoding of a sequence of characters.
        *
        * @param in_a First byte to decode.
        * @param in_b Second byte to decode.
        * @param in_c Third byte to decode.
        * @param in_d Fourth byte to decode.
        * @param output A vector to receive 3 Base64-decoded bytes.
        */
        inline bool decode_quadruplet(char in_a, char in_b, char in_c, char in_d, byte_vector& out)
        {
            auto out_a = static_cast<quadruplet_type>(decode_table[in_a]);
            auto out_b = static_cast<quadruplet_type>(decode_table[in_b]);
            auto out_c = static_cast<quadruplet_type>(decode_table[in_c]);
            auto out_d = static_cast<quadruplet_type>(decode_table[in_d]);

            PERSISTENCE_IF_UNLIKELY(((out_a | out_b | out_c | out_d) & ~static_cast<quadruplet_type>(0x3F)) != 0) {
                return false;
            }

            quadruplet_type concat_bytes = (out_a << 18) | (out_b << 12) | (out_c << 6) | out_d;
            decode_quadruplet(concat_bytes, out);
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
        output.reserve(quadruplets * 3);

        const char* p = input.data();
        for (std::size_t i = 1; i < quadruplets; ++i) {
            char a = *p++;
            char b = *p++;
            char c = *p++;
            char d = *p++;

            // process all but last quadruplet
            bool result = decode_quadruplet(a, b, c, d, output);
            if (!result) {
                return false;
            }
        }

        // process last quadruplet
        if (input[input.size() - 2] == '=') {
            bool result = decode_quadruplet(
                input[input.size() - 4],
                input[input.size() - 3],
                'A',
                'A',
                output
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
                output
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
                output
            );
            if (!result) {
                return false;
            }
        }

        return true;
    }
}
