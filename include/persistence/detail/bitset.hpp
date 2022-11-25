#pragma once
#include <stdexcept>

namespace persistence
{
    /**
     * Represents a fixed-size sequence of N bits.
     * This implementation is a backport of std::bitset to C++17 with all constexpr functions.
     * @tparam N The number of bits to store.
     */
    template<std::size_t N>
    struct BitSet
    {
        std::array<bool, N> values = { false };

        /** Returns the number of bits set to true. */
        constexpr std::size_t count() const
        {
            std::size_t count = 0;
            for (std::size_t k = 0; k < N; ++k) {
                if (values[k]) {
                    ++count;
                }
            }
            return count;
        }

        /** Returns the state of a bit at the given index. */
        constexpr bool get(std::size_t k) const
        {
            return values[k];
        }

        /** Sets the state of a bit at the given index. */
        constexpr void set(std::size_t k)
        {
            values[k] = true;
        }

        /** Returns the index of the first bit that is non-zero. */
        constexpr std::size_t first_set() const
        {
            for (std::size_t k = 0; k < N; ++k) {
                if (values[k]) {
                    return k;
                }
            }
            throw std::out_of_range("all values are zero");
        }

        /** Returns the index of the first bit that is zero. */
        constexpr std::size_t first_unset() const
        {
            for (std::size_t k = 0; k < N; ++k) {
                if (!values[k]) {
                    return k;
                }
            }
            throw std::out_of_range("all values are non-zero");
        }

        /**
         * Returns the index of the n-th non-zero bit.
         * @param index A zero-based index that specifies the n-th non-zero bit to find.
         */
        constexpr std::size_t pick(std::size_t index) const
        {
            std::size_t i = 0;
            for (std::size_t k = 0; k < N; ++k) {
                if (values[k]) {
                    if (i == index) {
                        return k;
                    }
                    ++i;
                }
            }
            throw std::out_of_range("index out of bounds");
        }
    };
}
