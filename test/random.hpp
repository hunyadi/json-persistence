#pragma once
#include <random>

namespace test
{
    template<typename Engine>
    bool random_bool(Engine& engine)
    {
        std::uniform_int_distribution<> distribution(0, 1);
        return distribution(engine);
    }

    template<typename T, typename Engine>
    T random_integer(Engine& engine)
    {
        std::uniform_int_distribution<T> distribution(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        return distribution(engine);
    }

    template<typename T, typename Engine>
    T random_integer(Engine& engine, T lower, T upper)
    {
        std::uniform_int_distribution<T> distribution(lower, upper);
        return distribution(engine);
    }

    template<typename Engine, typename Generator>
    auto random_items(Engine& engine, Generator g, std::size_t lower_count, std::size_t upper_count)
    {
        std::vector<decltype(g(engine))> items;
        std::uniform_int_distribution<std::size_t> distribution(lower_count, upper_count);
        std::size_t count = distribution(engine);
        for (std::size_t k = 0; k < count; ++k) {
            items.push_back(g(engine));
        }
        return items;
    }
}
