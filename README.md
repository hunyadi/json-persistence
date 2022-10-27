# Type-safe JSON persistence for C++17

This header-only C++17 library provides type-safe serialization of C++ objects into JSON strings, and deserialization of JSON strings to C++ objects.

## Features

* Header-only.
* Compile-time correctness.
* (De-)serialize fundamental types `bool`, `short`, `int`, `long`, `float`, `double`, etc. to/from their respective JSON type.
* (De-)serialize `time_point<system_clock>` type as ISO 8601 date-time string in time zone UTC.
* (De-)serialize enumeration types as their underlying integer type.
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
    auto json = serialize_to_string(obj);
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

### Speed and efficiency

* Built on top of [RapidJSON](https://rapidjson.org/).
* Uses RapidJSON [SAX interface](https://rapidjson.org/md_doc_sax.html) for writing and parsing JSON strings directly, bypassing the JSON DOM.
* Uses a polymorphic stack to reduce dynamic memory allocations on heap.

### Platform-neutral

* Uses standard C++17 features only (with the exception of RapidJSON engine).
* Compiles on Linux, MacOS and Windows.
