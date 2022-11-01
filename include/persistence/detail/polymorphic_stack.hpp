#pragma once
#include <cstddef>
#include <stdexcept>
#include <type_traits>

namespace persistence
{
    namespace detail
    {
        /**
        * A stack for storing polymorphic objects deriving from a common base class.
        *
        * @tparam Base Type that all polymorphic objects derive from.
        */
        template<typename Base, std::size_t Capacity = 128, std::size_t MemorySize = 4096>
        class PolymorphicStack
        {
        public:
            static_assert(std::has_virtual_destructor<Base>::value, "base class requires a virtual destructor for proper de-allocation");

            template<typename Class, typename... T>
            Base& emplace(T&&... args)
            {
                static_assert(std::is_base_of<Base, Class>::value, "newly constructed class must derive from base class");

                if (count >= Capacity || next + alignof(Class) + sizeof(Class) >= memory + MemorySize) {
                    throw std::out_of_range("stack space exhausted");
                }

                items[count] = current;
                ++count;

                // ensure proper alignment for newly constructed object
                current = reinterpret_cast<std::byte*>(new (next) Class(std::forward<T>(args)...));
                next = current + sizeof(Class);

                return back();
            }

            void pop()
            {
                if (count == 0) {
                    throw std::out_of_range("empty stack");
                }

                pop_unsafe();
            }

            Base& back()
            {
                return *reinterpret_cast<Base*>(current);
            }

            ~PolymorphicStack()
            {
                while (count > 0) {
                    pop_unsafe();
                }
            }

        private:
            void pop_unsafe()
            {
                --count;
                reinterpret_cast<Base*>(current)->~Base();
                next = current;
                current = items[count];
                items[count] = nullptr;
            }

        private:
            std::byte memory[MemorySize];
            std::byte* current = nullptr;
            std::byte* next = memory;
            std::byte* items[Capacity] = { nullptr };
            std::size_t count = 0;
        };
    }
}
