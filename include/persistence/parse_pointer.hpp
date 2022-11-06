#pragma once
#include "parse_base.hpp"
#include <memory>

namespace persistence
{
    template<typename T>
    struct JsonParser<T*>
    {
        static_assert(detail::fail<T>, "parsing raw pointers is not supported, use smart pointers instead");
    };

    template<typename T>
    struct JsonParser<std::unique_ptr<T>> : JsonParser<T>
    {
        JsonParser(ReaderContext& context, std::unique_ptr<T>& ref)
            : JsonParser<T>(context, assign_unique(ref))
        {}

    private:
        static T& assign_unique(std::unique_ptr<T>& ref)
        {
            ref = std::make_unique<T>();
            return *ref;
        }
    };

    template<typename T>
    struct JsonParser<std::shared_ptr<T>> : JsonParser<T>
    {
        JsonParser(ReaderContext& context, std::shared_ptr<T>& ref)
            : JsonParser<T>(context, assign_shared(ref))
        {}

    private:
        static T& assign_shared(std::shared_ptr<T>& ref)
        {
            ref = std::make_shared<T>();
            return *ref;
        }
    };
}
