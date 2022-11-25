#pragma once
#include "bytes.hpp"
#include "enum.hpp"
#include "datetime.hpp"
#include "dictionary.hpp"
#include "object.hpp"
#include "detail/defer.hpp"
#include "detail/traits.hpp"
#include <set>

namespace persistence
{
    struct JsonSchemaReferenceType;
    struct JsonSchemaFundamentalType;
    struct JsonSchemaSignedIntegerType;
    struct JsonSchemaUnsignedIntegerType;
    struct JsonSchemaEnumType;
    struct JsonSchemaArrayType;
    struct JsonSchemaObjectType;
    struct JsonSchemaOneOfType;

    using JsonSchemaType = std::variant<
        JsonSchemaReferenceType,
        JsonSchemaFundamentalType,
        JsonSchemaSignedIntegerType,
        JsonSchemaUnsignedIntegerType,
        JsonSchemaEnumType,
        JsonSchemaArrayType,
        JsonSchemaObjectType,
        JsonSchemaOneOfType
    >;

    struct JsonSchemaReferenceType
    {
        JsonSchemaReferenceType() = default;
        JsonSchemaReferenceType(const std::string& ref)
            : ref(ref)
        {}

        std::string ref;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & NAMED_MEMBER_VARIABLE("$ref", ref)
                ;
        }
    };

    namespace encoding
    {
        struct Base64
        {
            constexpr static const char* identifier = "base64";
        };
    }

    namespace format
    {
        struct Date
        {
            constexpr static const char* identifier = "date";
        };

        struct DateTime
        {
            constexpr static const char* identifier = "date-time";
        };
    }

    struct JsonSchemaFundamentalType
    {
        JsonSchemaFundamentalType() = default;

        JsonSchemaFundamentalType(const std::string_view& type)
            : type(type)
        {}

        JsonSchemaFundamentalType(encoding::Base64)
            : type("string")
            , contentEncoding(encoding::Base64::identifier)
        {}

        JsonSchemaFundamentalType(format::DateTime)
            : type("string")
            , format(format::DateTime::identifier)
        {}

        JsonSchemaFundamentalType(format::Date)
            : type("string")
            , format(format::Date::identifier)
        {}

        std::string_view type;
        std::optional<std::string_view> format;
        std::optional<std::string_view> contentEncoding;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & MEMBER_VARIABLE(type)
                & MEMBER_VARIABLE(format)
                & MEMBER_VARIABLE(contentEncoding)
                ;
        }
    };

    struct JsonSchemaSignedIntegerType
    {
        JsonSchemaSignedIntegerType(long long minimum, long long maximum)
            : minimum(minimum)
            , maximum(maximum)
        {}

        std::string_view type = "integer";
        long long minimum;
        long long maximum;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & MEMBER_VARIABLE(type)
                & MEMBER_VARIABLE(minimum)
                & MEMBER_VARIABLE(maximum)
                ;
        }
    };

    struct JsonSchemaUnsignedIntegerType
    {
        JsonSchemaUnsignedIntegerType(unsigned long long maximum)
            : maximum(maximum)
        {}

        std::string_view type = "integer";
        unsigned long long minimum = 0;
        unsigned long long maximum;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & MEMBER_VARIABLE(type)
                & MEMBER_VARIABLE(minimum)
                & MEMBER_VARIABLE(maximum)
                ;
        }
    };

    struct JsonSchemaEnumType
    {
        JsonSchemaEnumType(const std::string_view& type)
            : type(type)
        {}

        std::string_view type;
        std::variant<
            std::vector<long long>,
            std::vector<unsigned long long>,
            std::vector<std::string>
        > enum_values;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & MEMBER_VARIABLE(type)
                & NAMED_MEMBER_VARIABLE("enum", enum_values)
                ;
        }
    };

    struct JsonSchemaArrayType
    {
        std::string_view type = "array";
        std::optional<std::unique_ptr<JsonSchemaType>> items;
        std::optional<std::vector<std::unique_ptr<JsonSchemaType>>> prefixItems;
        std::optional<bool> uniqueItems;
        std::optional<std::size_t> minItems;
        std::optional<std::size_t> maxItems;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & MEMBER_VARIABLE(type)
                & MEMBER_VARIABLE(items)
                & MEMBER_VARIABLE(prefixItems)
                & MEMBER_VARIABLE(uniqueItems)
                & MEMBER_VARIABLE(minItems)
                & MEMBER_VARIABLE(maxItems)
                ;
        }
    };

    using JsonSchemaObjectProperties = literal_dict<std::unique_ptr<JsonSchemaType>>;
    using JsonSchemaObjectRequired = std::vector<std::string_view>;

    struct JsonSchemaObjectType
    {
        std::string_view type = "object";
        std::optional<JsonSchemaObjectProperties> properties;
        std::optional<JsonSchemaObjectRequired> required;
        std::optional<std::variant<bool, std::unique_ptr<JsonSchemaType>>> additionalProperties;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & MEMBER_VARIABLE(type)
                & MEMBER_VARIABLE(properties)
                & MEMBER_VARIABLE(required)
                & MEMBER_VARIABLE(additionalProperties)
                ;
        }
    };

    struct JsonSchemaOneOfType
    {
        std::vector<std::unique_ptr<JsonSchemaType>> oneOf;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & MEMBER_VARIABLE(oneOf)
                ;
        }
    };

    using JsonSchemaDefinitions = std::map<std::string_view, JsonSchemaType>;

    struct JsonSchemaContext
    {
        JsonSchemaDefinitions definitions;
    };

    template<typename T, typename Enable = void>
    struct JsonSchema
    {};

    template<>
    struct JsonSchema<std::nullptr_t>
    {
        static auto type()
        {
            return JsonSchemaFundamentalType("null");
        }
    };

    template<>
    struct JsonSchema<bool>
    {
        static auto type()
        {
            return JsonSchemaFundamentalType("boolean");
        }
    };

    template<typename T>
    struct JsonSchema<T, std::enable_if_t<std::is_integral_v<T>&& std::is_signed_v<T>>>
    {
        static auto type()
        {
            return JsonSchemaSignedIntegerType(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        }
    };

    template<typename T>
    struct JsonSchema<T, std::enable_if_t<std::is_integral_v<T> && !std::is_signed_v<T>>>
    {
        static auto type()
        {
            return JsonSchemaUnsignedIntegerType(std::numeric_limits<T>::max());
        }
    };

    template<typename T>
    struct JsonSchema<T, std::enable_if_t<std::is_floating_point_v<T>>>
    {
        static auto type()
        {
            return JsonSchemaFundamentalType("number");
        }
    };

    template<>
    struct JsonSchema<std::string>
    {
        static auto type()
        {
            return JsonSchemaFundamentalType("string");
        }
    };

    template<>
    struct JsonSchema<byte_vector>
    {
        static auto type()
        {
            return JsonSchemaFundamentalType(encoding::Base64());
        }
    };

    template<>
    struct JsonSchema<timestamp>
    {
        static auto type()
        {
            return JsonSchemaFundamentalType(format::DateTime());
        }
    };

#if __cplusplus >= 202002L
    template<>
    struct JsonSchema<std::chrono::year_month_day>
    {
        static auto type()
        {
            return JsonSchemaFundamentalType(format::Date());
        }
    };
#endif

    template<typename T>
    struct JsonSchema<T, std::enable_if_t<std::is_enum_v<T>>>
    {
        static auto type()
        {
            if constexpr (detect<T, enum_to_string_function>::value) {
                return JsonSchemaEnumType("string");
            } else if constexpr (std::is_integral_v<T>) {
                return JsonSchemaEnumType("integer");
            } else {
                return JsonSchemaEnumType("object");
            }
        }
    };

    template<typename C>
    struct JsonSchemaVisitor;

    template<typename T>
    using schema_function = decltype(std::declval<T&>().persist(std::declval<JsonSchemaVisitor<T>&>()));

    /** .True if the sub-schema can be registered as a named type. */
    template<typename T, typename Enable = void>
    struct is_schema_traversable : std::false_type
    {};

    template<typename T>
    struct is_schema_traversable<T, std::enable_if_t<std::is_class_v<T>>>
    {
        constexpr static bool value = detect<T, schema_function>::value;
    };

    template<typename T>
    inline constexpr bool is_schema_traversable_v = is_schema_traversable<T>::value;

    /** True if the sub-schema uses types from an outside context. */
    template<typename T>
    using context_aware_schema_function = decltype(JsonSchema<T>::type(std::declval<JsonSchemaContext&>()));

    template<typename T>
    struct is_context_aware_schema
    {
        constexpr static bool value = detect<T, context_aware_schema_function>::value;
    };

    template<typename T>
    inline constexpr bool is_context_aware_schema_v = is_context_aware_schema<T>::value;

    template<typename T>
    using context_free_schema_function = decltype(JsonSchema<T>::type());

    /** True if the sub-schema does not use types from an outside context. */
    template<typename T>
    struct is_context_free_schema
    {
        constexpr static bool value = detect<T, context_free_schema_function>::value;
    };

    template<typename T>
    inline constexpr bool is_context_free_schema_v = is_context_free_schema<T>::value;

    template<typename T>
    JsonSchemaType make_schema(JsonSchemaContext& context)
    {
        if constexpr (is_schema_traversable_v<T>) {
            // save sub-schema in context
            context.definitions.insert(
                std::make_pair(
                    std::string_view(typeid(T).name()),
                    JsonSchema<T>::type(context)
                )
            );

            // use alias to refer to sub-schema
            return JsonSchemaReferenceType(
                "#/definitions/" + std::string(typeid(T).name())
            );
        } else if constexpr (is_context_aware_schema_v<T>) {
            return JsonSchema<T>::type(context);
        } else {
            return JsonSchema<T>::type();
        }
    }

    template<typename T>
    std::unique_ptr<JsonSchemaType> make_schema_ptr(JsonSchemaContext& context)
    {
        return std::make_unique<JsonSchemaType>(make_schema<T>(context));
    }

    template<typename C>
    struct JsonSchemaVisitor
    {
        JsonSchemaVisitor(JsonSchemaContext& context, JsonSchemaObjectProperties& properties, JsonSchemaObjectRequired& required)
            : context(context)
            , properties(properties)
            , required(required)
        {}

        template<typename T, class B, auto P>
        JsonSchemaVisitor& operator&(const member_variable<std::optional<T>, B, P>& member)
        {
            static_assert(std::is_base_of_v<B, C>, "expected a member variable part of the class inheritance chain");
            properties.push_back(
                std::make_pair(member.name(), make_schema_ptr<T>(context))
            );
            return *this;
        }

        template<typename T, class B, auto P>
        JsonSchemaVisitor& operator&(const member_variable<T, B, P>& member)
        {
            static_assert(std::is_base_of_v<B, C>, "expected a member variable part of the class inheritance chain");
            properties.push_back(
                std::make_pair(member.name(), make_schema_ptr<T>(context))
            );
            required.push_back(member.name());
            return *this;
        }

        JsonSchemaContext& context;
        JsonSchemaObjectProperties& properties;
        JsonSchemaObjectRequired& required;
    };

    template<typename T>
    struct JsonSchema<T, std::enable_if_t<is_schema_traversable_v<T>>>
    {
        static JsonSchemaObjectType type(JsonSchemaContext& context)
        {
            auto schema = JsonSchemaObjectType();
            schema.properties = JsonSchemaObjectProperties();
            schema.required = JsonSchemaObjectRequired();
            schema.additionalProperties = false;
            JsonSchemaVisitor<T> visitor(context, *schema.properties, *schema.required);
            T().persist(visitor);
            return schema;
        }
    };

    template<typename T>
    struct JsonSchema<T*>
    {
        static_assert(detail::fail<T>, "generating schema for raw pointers is not supported, use smart pointers instead");
    };

    template<typename T>
    struct JsonSchema<std::unique_ptr<T>>
    {
        static auto type(JsonSchemaContext& context)
        {
            return JsonSchema<T>::type(context);
        }
    };

    template<typename T>
    struct JsonSchema<std::shared_ptr<T>>
    {
        static auto type(JsonSchemaContext& context)
        {
            return JsonSchema<T>::type(context);
        }
    };

    template<typename T, std::size_t N>
    struct JsonSchema<std::array<T, N>>
    {
        static JsonSchemaArrayType type(JsonSchemaContext& context)
        {
            auto schema = JsonSchemaArrayType();
            schema.items = make_schema_ptr<T>(context);
            schema.minItems = N;
            schema.maxItems = N;
            return schema;
        }
    };

    template<typename... T>
    struct JsonTupleSchema
    {
        static JsonSchemaArrayType type(JsonSchemaContext& context)
        {
            auto schema = JsonSchemaArrayType();
            schema.minItems = sizeof...(T);
            schema.maxItems = sizeof...(T);
            std::vector<std::unique_ptr<JsonSchemaType>> items;
            (items.push_back(make_schema_ptr<T>(context)), ...);
            schema.prefixItems = std::move(items);
            return schema;
        }
    };

    template<typename T1, typename T2>
    struct JsonSchema<std::pair<T1, T2>> : JsonTupleSchema<T1, T2>
    {};

    template<typename... T>
    struct JsonSchema<std::tuple<T...>> : JsonTupleSchema<T...>
    {};

    template<typename... T>
    struct JsonSchema<std::variant<T...>>
    {
        static JsonSchemaOneOfType type(JsonSchemaContext& context)
        {
            auto schema = JsonSchemaOneOfType();
            std::vector<std::unique_ptr<JsonSchemaType>> items;
            (items.push_back(make_schema_ptr<T>(context)), ...);
            schema.oneOf = std::move(items);
            return schema;
        }
    };

    template<typename T>
    struct JsonSchema<std::vector<T>>
    {
        static JsonSchemaArrayType type(JsonSchemaContext& context)
        {
            auto schema = JsonSchemaArrayType();
            schema.items = make_schema_ptr<T>(context);
            return schema;
        }
    };

    template<typename T>
    struct JsonSchema<std::set<T>>
    {
        static JsonSchemaArrayType type(JsonSchemaContext& context)
        {
            auto schema = JsonSchemaArrayType();
            schema.items = make_schema_ptr<T>(context);
            schema.uniqueItems = true;
            return schema;
        }
    };

    template<typename MapType>
    struct JsonMapSchema
    {
        static JsonSchemaObjectType type(JsonSchemaContext& context)
        {
            auto schema = JsonSchemaObjectType();
            schema.additionalProperties = make_schema_ptr<typename MapType::mapped_type>(context);
            return schema;
        }
    };

    template<typename T>
    struct JsonSchema<std::map<std::string, T>> : JsonMapSchema<std::map<std::string, T>>
    {};

    template<typename T>
    struct JsonSchema<std::unordered_map<std::string, T>> : JsonMapSchema<std::unordered_map<std::string, T>>
    {};

    template<typename Base>
    struct JsonSchemaDocument : Base
    {
        JsonSchemaDocument(Base&& type)
            : Base(std::move(type))
        {}

        JsonSchemaDocument(std::string_view schema_version, Base&& type)
            : Base(std::move(type))
            , schema_version(schema_version)
        {}

        JsonSchemaDocument(JsonSchemaDefinitions&& definitions, Base&& type)
            : Base(std::move(type))
            , definitions(std::move(definitions))
        {}

        JsonSchemaDocument(std::string_view schema_version, JsonSchemaDefinitions&& definitions, Base&& type)
            : Base(std::move(type))
            , definitions(std::move(definitions))
            , schema_version(schema_version)
        {}

        std::optional<std::string_view> schema_version;
        std::optional<JsonSchemaDefinitions> definitions;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            auto r = ar
                & NAMED_MEMBER_VARIABLE("$schema", schema_version)
                & MEMBER_VARIABLE(definitions)
                ;
            return this->Base::persist(r);
        }
    };

    template<typename T, typename Enable = void>
    struct JsonSchemaBuilder
    {
        // if you are getting a compile-time error pointing at this location, make sure the appropriate
        // headers are included, and schema generation is supported for the type
        static_assert(detail::fail<T>, "expected a type that can be converted to a JSON Schema");
    };

    template<typename T>
    struct JsonSchemaBuilder<T, std::enable_if_t<is_context_free_schema_v<T>>>
    {
        auto build()
        {
            auto type = JsonSchema<T>::type();
            return JsonSchemaDocument<decltype(type)>(std::move(type));
        }
    };

    template<typename T>
    struct JsonSchemaBuilder<T, std::enable_if_t<is_context_aware_schema_v<T>>>
    {
        auto build()
        {
            JsonSchemaContext context;
            auto type = JsonSchema<T>::type(context);
            if (!context.definitions.empty()) {
                return JsonSchemaDocument<decltype(type)>(std::move(context.definitions), std::move(type));
            } else {
                return JsonSchemaDocument<decltype(type)>(std::move(type));
            }
        }
    };

    template<typename T>
    auto schema()
    {
        return JsonSchemaBuilder<T>().build();
    }

    template<typename T>
    auto schema_document()
    {
        auto doc = schema<T>();
        doc.schema_version = "https://json-schema.org/draft/2020-12/schema";
        return doc;
    }
}
