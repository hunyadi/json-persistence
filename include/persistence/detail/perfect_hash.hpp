#pragma once
#include "bitset.hpp"
#include <string_view>

namespace persistence
{
    namespace detail
    {
        using hash_t = std::size_t;
    
        /** Fowler–Noll–Vo hash function variant FNV-1. */
        inline constexpr hash_t hash(hash_t d, const std::string_view& str)
        {
            if constexpr (sizeof(hash_t) == sizeof(std::uint32_t)) {
                // Fowler–Noll–Vo hash function variant FNV-1 for 32 bits
                if (d == 0) {
                    d = 0x811c9dc5;
                }

                for (std::size_t i = 0; i < str.length(); ++i) {
                    // multiply by the 32 bit FNV magic prime mod 2^32
                    d += (d << 1) + (d << 4) + (d << 7) + (d << 8) + (d << 24);
                    // xor the bottom with the current octet
                    d ^= str[i];
                }
                return d & 0x7fffffff;
            } else if constexpr (sizeof(hash_t) == sizeof(std::uint64_t)) {
                // Fowler–Noll–Vo hash function variant FNV-1 for 64 bits
                if (d == 0) {
                    d = 0xcbf29ce484222325;
                }

                for (std::size_t i = 0; i < str.length(); ++i) {
                    // multiply by the 64 bit FNV magic prime mod 2^64
                    d += (d << 1) + (d << 4) + (d << 5) + (d << 7) + (d << 8) + (d << 40);
                    // xor the bottom with the current octet
                    d ^= str[i];
                }
                return d & 0x7fffffffffffffff;
            }
        }
    }

    /** A lookup table with perfect hashing. */
    template<std::size_t Size>
    struct PerfectHash
    {
        constexpr PerfectHash(const std::string_view (&items)[Size])
            : PerfectHash(items, Size)
        {}

        constexpr PerfectHash(const std::array<std::string_view, Size>& items)
            : PerfectHash(items, Size)
        {}

        /**
         * Finds the index of an element in the array using perfect hash lookup.
         *
         * @param key The value of an array element whose index to find.
         * @returns The index of the element, or undefined if the element is not in the array.
         */
        constexpr std::size_t index(const std::string_view& key) const
        {
            using namespace detail;

            hash_t d = G[hash(0, key) % Size];
            constexpr hash_t first_bit = ~(~hash_t() >> 1);
            if ((d & first_bit) != 0) {
                hash_t di = ~d;
                return index_map[di];
            } else {
                return index_map[hash(d, key) % Size];
            }
        }

    private:
        template<typename Indexable>
        constexpr PerfectHash(const Indexable& items, std::size_t)
        {
            using namespace detail;

            // place all of the items into buckets
            BitSet<Size> buckets[Size];
            for (std::size_t k = 0; k < Size; ++k) {
                std::size_t key = hash(0, items[k]) % Size;
                buckets[key].set(k);
            }

            // process the buckets with the most items first (but at least 2)
            BitSet<Size> taken;
            for (std::size_t count = Size; count > 1; --count) {
                for (std::size_t i = 0; i < Size; ++i) {
                    if (buckets[i].count() != count) {
                        continue;
                    }

                    // find a seed that places all items into free slots
                    hash_t d = scatter(items, buckets[i], taken);
                    G[hash(0, items[buckets[i].first_set()]) % Size] = d;

                    // assign items to free slots
                    for (std::size_t k = 0; k < count; ++k) {
                        auto item_index = buckets[i].pick(k);
                        auto bucket_item = items[item_index];

                        hash_t index = hash(d, bucket_item) % Size;

                        taken.set(index);
                        index_map[index] = item_index;
                    }
                }
            }

            // process buckets with exactly 1 item by directly placing them into a free slot
            for (std::size_t i = 0; i < Size; ++i) {
                if (buckets[i].count() == 1) {
                    auto item_index = buckets[i].first_set();
                    auto bucket_item = items[item_index];

                    std::size_t index = taken.first_unset();

                    // ones' complement indicates that this fast path was taken
                    G[hash(0, bucket_item) % Size] = ~static_cast<hash_t>(index);

                    taken.set(index);
                    index_map[index] = item_index;
                }
            }
        }

        /**
         * Repeatedly tries different seed values until it finds one that places
         * all items into different free slots.
         *
         * @param items An array of items to place into free slots.
         * @param mask A mask that identifies which items in the array to take into account.
         * @param taken A mask that identifies occupied slots.
         */
        template<typename Indexable>
        constexpr detail::hash_t scatter(const Indexable& items, const BitSet<Size>& mask, const BitSet<Size>& taken)
        {
            detail::hash_t d = 1;
            while (!is_distributable(d, items, mask, taken)) {
                ++d;
            }
            return d;
        }

        /**
         * Checks if a seed value places all items into different free slots.
         *
         * @param d The seed value to test.
         * @param items An array of items to place into free slots.
         * @param mask A mask that identifies which items in the array to take into account.
         * @param taken A mask that identifies occupied slots.
         */
        template<typename Indexable>
        constexpr bool is_distributable(detail::hash_t d, const Indexable& items, const BitSet<Size>& mask, BitSet<Size> taken)
        {
            for (std::size_t k = 0; k < Size; ++k) {
                if (!mask.get(k)) {
                    continue;
                }

                auto key = detail::hash(d, items[k]) % Size;
                if (taken.get(key)) {
                    return false;
                }
                taken.set(key);
            }
            return true;
        }

    private:
        std::size_t index_map[Size] = { 0 };
        detail::hash_t G[Size] = { 0 };
    };
}
