# Type-safe JSON persistence for C++17

This header-only C++17 library provides type-safe serialization of C++ objects into JSON strings, and deserialization of JSON strings to C++ objects.

## Features

* Header-only.
* Compile-time correctness.
* (De-)serialize fundamental types `bool`, `short`, `int`, `long`, `float`, `double`, etc. to/from their respective JSON type.
* (De-)serialize `time_point<system_clock>` type to/from an ISO 8601 date-time string in time zone UTC.
* (De-)serialize enumeration types as their underlying integer type or as string (with auxiliary helper functions `to_string` and `from_string`).
* (De-)serialize `vector<std::byte>` type to/from a Base64-encoded string.
* (De-)serialize heterogeneous `pair<T1,T2>` and `tuple<T...>` to/from a JSON array.
* (De-)serialize container types `vector<T>`, `array<T, N>`, `set<T>`, etc. to/from a JSON array.
* (De-)serialize dictionary types `map<string, T>` and `unordered_map<string, T>` to/from a JSON object.
* Serialize variant types to their stored type. De-serialize variant types using the first matching type.
* (De-)serialize class types by enumerating their member variables.
* Omit object fields for missing value in `optional<T>`.
* (De-)serialize pointer types such as `shared_ptr<T>` and `unique_ptr<T>`.
* Back-reference support for (de-)serialization of C++ pointer types via JSON pointer `{"$ref": "/path/to/previous/occurrence"}`.
* Serialize C++ objects that share pointers only once. De-serialize back-references as C++ pointers to the same object.

## Design goals

### Intuitive syntax

1. Define member variables for JSON serialization and de-serialization within the class definition:

    ```cpp
    struct Example
    {
        bool bool_value = false;
        std::string string_value;
        std::vector<std::string> string_list;
        std::optional<int> optional_value;
        UserDefinedType custom_value;

        template <typename Archive>
        constexpr auto persist(Archive& ar)
        {
            return ar
                & MEMBER_VARIABLE(bool_value)
                & MEMBER_VARIABLE(string_value)
                & MEMBER_VARIABLE(string_list)
                & MEMBER_VARIABLE(optional_value)
                & MEMBER_VARIABLE(custom_value)
                ;
        }
    };
    ```

2. Serialize an object to a JSON string:

    ```cpp
    Example obj = { true, "string", {"a","b","c"}, std::nullopt, {"xyz"} };
    auto json = write_to_string(obj);
    ```

    in which the variable `json` holds:

    ```json
    {
        "bool_value": true,
        "string_value": "string",
        "string_list": ["a", "b", "c"],
        "custom_value": {"value": "xyz"}
    }
    ```

3. Parse a JSON string into an object of the given type:

    ```cpp
    Example res = parse<Example>(json);
    ```

### Ease of use

* Copy the `include` folder to your project's header include path, no build or installation required.
* Get started by including `<persistence/persistence.hpp>`, which enables all features.
* Selectively include headers such as `<persistence/write_object.hpp>` to tackle a specific use case:
    * Use `<persistence/write_*.hpp>` for writing a C++ object to a string.
    * Use `<persistence/parse_*.hpp>` for parsing a string into a C++ object.
    * Use `<persistence/serialize_*.hpp>` for serializing a C++ object to a JSON DOM document.
    * Use `<persistence/deserialize_*.hpp>` for parsing a JSON DOM document into a C++ object.
* Use `<persistence/*_all.hpp>` to import all supported data types for an operation type.
* Use `<persistence/object.hpp>` to import helper macro `MEMBER_VARIABLE` only.
* Use `<persistence/utility.hpp>` to import helper functions to convert between JSON DOM document and JSON string.

### Extensible

Add new template specializations to support (de-)serializing new types:

* Specialize `JsonWriter<T>` to support writing new C++ types to JSON string.
* Specialize `JsonParser<T>` to support parsing new C++ types from JSON string.
* Specialize `JsonSerializer<T>` to serialize new C++ types to JSON DOM.
* Specialize `JsonDeserializer<T>` to deserialize new C++ types from JSON DOM.

(De-)serialize, parse and write enumeration types as strings by defining value-to-string and string-to-value conversions:

```cpp
namespace persistence
{
    template<>
    struct enum_traits<MyEnum>
    {
        static std::string_view to_string(MyEnum value)
        {
            // return a distinct string literal for each enumeration value
            return std::string_view();
        }

        static bool from_string(const std::string_view& name, MyEnum& value)
        {
            // assign enumeration value based on string literal
            value = MyEnum();

            // return true if value has been assigned; false on no match
            return true;
        }
    };
}
```

### Speed and efficiency

* Built on top of [RapidJSON](https://rapidjson.org/).
* Uses RapidJSON [SAX interface](https://rapidjson.org/md_doc_sax.html) for writing and parsing JSON strings directly, bypassing the JSON DOM.
* Uses a polymorphic stack to reduce dynamic memory allocations on heap.
* Unrolls loops at compile-time for bounded-length data structures such as pairs, tuples and object properties.

### Platform-neutral

* Uses standard C++17 features only (with the exception of RapidJSON engine).
* Compiles on Linux, MacOS and Windows.

## Limitations and workarounds

JSON writer and parser does not support back-references (`{"$ref": "/path/to/previous/occurrence"}`). Pointer-like types always create a new instance. Circular references will lead to an infinite loop. When using back-references,

* serialize the data to JSON DOM, and then use the utility function `document_to_string` to write the JSON DOM as a JSON string;
* read the JSON string into a JSON DOM with the utility function `string_to_document`, and then de-serialize the data from JSON DOM.

Parsing and de-serializing raw pointers is not permitted due to lack of clarity around ownership. Use `unique_ptr` and `smart_ptr` instead. Writing and serializing raw pointers is allowed.

JSON parser does not support variant types. Instead, read the JSON string into a JSON DOM with the utility function `string_to_document`, and then de-serialize the data from JSON DOM. JSON writer, serializer and de-serializer support variant types.
