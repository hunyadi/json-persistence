#pragma once
#include <string_view>
#include <type_traits>

namespace persistence
{
    namespace member
    {
        /**
         * Represents a member variable in a (de-)serialization annotation.
         *
         * @tparam Type The type of the member variable bound by this structure.
         * @tparam Class The type of the encapsulating object.
         * @tparam Pointer The member pointer value.
         */
        template<typename Type, class Class, Type Class::* Pointer, typename Descriptor>
        struct variable_annotation
        {
            using pointer_type = decltype(Pointer);
            /** The type of the member variable bound by this structure. */
            using member_type = Type;
            /** The type of the encapsulating object. */
            using class_type = Class;

            static_assert(std::is_class_v<class_type>, "expected a user-defined class type");
            static_assert(std::is_member_object_pointer_v<pointer_type>, "expected a non-static member object pointer");

            /**
             * The name of the member variable as a constant string literal.
             */
            constexpr std::string_view name() const
            {
                return Descriptor::name();
            }

            /**
             * The member variable pointer.
             */
            constexpr pointer_type pointer() const
            {
                return Pointer;
            }

            /**
             * A writable reference to the member variable value.
             */
            member_type& ref(class_type& obj) const
            {
                return obj.*Pointer;
            }

            /**
             * A read-only reference to the member variable value.
             */
            const member_type& ref(const class_type& obj) const
            {
                return obj.*Pointer;
            }

        protected:
            constexpr variable_annotation() = default;
        };

        /**
         * Represents a member variable in a (de-)serialization annotation that has no default value.
         *
         * @tparam Type The type of the member variable bound by this structure.
         * @tparam Class The type of the encapsulating object.
         * @tparam Pointer The member pointer value.
         */
        template<typename Type, class Class, auto Pointer, typename Descriptor>
        struct variable final : variable_annotation<Type, Class, Pointer, Descriptor>
        {
            using variable_annotation<Type, Class, Pointer, Descriptor>::variable_annotation;
        };

        /**
         * Represents a member variable in a (de-)serialization annotation that has a default value assigned
         * in a constructor or by a member initializer.
         *
         * @tparam Type The type of the member variable bound by this structure.
         * @tparam Class The type of the encapsulating object.
         * @tparam Pointer The member pointer value.
         */
        template<typename Type, class Class, auto Pointer, typename Descriptor>
        struct variable_default final : variable_annotation<Type, Class, Pointer, Descriptor>
        {
            using variable_annotation<Type, Class, Pointer, Descriptor>::variable_annotation;
        };
    }
}
