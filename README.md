# Type-safe JSON persistence for C++17 and later

This header-only library for C++17 and later provides type-safe serialization of C++ objects into JSON strings, and deserialization of JSON strings to C++ objects.

## Features

* Header-only.
* Compile-time correctness.
* (De-)serialize fundamental types `bool`, `short`, `int`, `long`, `float`, `double`, etc. to/from their respective JSON type.
* (De-)serialize `time_point<system_clock>` type to/from an ISO 8601 date-time string in time zone UTC.
* (De-)serialize `year_month_day` type to/from an ISO 8601 date string (C++20 and later only).
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

### Easy to use

* Copy the `include` folder to your project's header include path, no build or installation required.
* Get started by including `<persistence/persistence.hpp>`, which enables all features.
* Selectively include headers such as `<persistence/write_object.hpp>` to tackle a specific use case:
    * Use `<persistence/write_*.hpp>` for writing a C++ object to a string.
    * Use `<persistence/parse_*.hpp>` for parsing a string into a C++ object.
    * Use `<persistence/serialize_*.hpp>` for serializing a C++ object to a JSON DOM document.
    * Use `<persistence/deserialize_*.hpp>` for de-serializing a JSON DOM document into a C++ object.
* Use `<persistence/*_all.hpp>` to import all supported data types for an operation type.
* Use `<persistence/object.hpp>` to import helper macro `MEMBER_VARIABLE` only.
* Use `<persistence/utility.hpp>` to import helper functions to convert between JSON DOM document and JSON string.

### Extensible

Add new template specializations to support (de-)serializing new types:

* Specialize `JsonWriter<T>` to support writing new C++ types to JSON string.
* Specialize `JsonParser<T>` to support parsing new C++ types from JSON string.
* Specialize `JsonSerializer<T>` to serialize new C++ types to JSON DOM.
* Specialize `JsonDeserializer<T>` to deserialize new C++ types from JSON DOM.

(De-)serialize, parse and write enumeration types as strings by defining value-to-string and string-to-value conversions.

### Fast and efficient

* Built on top of [RapidJSON](https://rapidjson.org/).
* Uses RapidJSON [SAX interface](https://rapidjson.org/md_doc_sax.html) for writing and parsing JSON strings directly, bypassing the JSON DOM.
* Uses a polymorphic stack to reduce dynamic memory allocations on heap.
* Unrolls loops at compile-time for bounded-length data structures such as pairs, tuples and object properties.

### Platform-neutral

* Uses standard C++17 features only (with the exception of RapidJSON engine).
* Compiles on Linux, MacOS and Windows.

## Data transformation modes

The library supports several transformation modes:

* writing a C++ object directly to a string (without JSON DOM):
    ```cpp
    std::string str = write_to_string(obj);
    ```
* parsing a string directly into a C++ object (without JSON DOM):
    ```cpp
    auto obj = parse<T>(str);
    ```
* serializing a C++ object to a JSON DOM document:
    ```cpp
    rapidjson::Document doc = serialize_to_document(obj);
    ```
* de-serializing a JSON DOM document into a C++ object:
    ```cpp
    auto obj = deserialize<T>(doc);
    ```
* writing a JSON DOM document to a string (with utility function `document_to_string`)
* parsing a JSON DOM document from a string (with utility function `string_to_document`)
* serializing a C++ object to a JSON DOM, and then writing the JSON DOM to a string:
    ```cpp
    rapidjson::Document doc = serialize_to_document(obj);
    std::string str = document_to_string(doc);
    ```
* reading a string into a JSON DOM, and then de-serializing the data from JSON DOM into a C++ object:
    ```cpp
    rapidjson::Document doc = string_to_document(str);
    auto obj = deserialize<T>(doc);
    ```

## Enumeration to string conversion

In order to (de-)serialize, parse and write enumeration types as strings, define value-to-string and string-to-value conversions with the type trait class `enum_traits`:

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

Use utility function `make_enum_converter` and class `EnumConverter<E, N>` to implement `to_string` and `from_string` with less boilerplate code.

## Error reporting

Functions that take both a source and a target object reference return a boolean result and throw no exceptions. Functions that take only a source argument and return a target object value throw exceptions on error.

Consider the following C++ code that invokes the parser:
```cpp
try {
    parse<Example>("{\"bool_value\": true, []}");
} catch (JsonParseError& e) {
    std::cout << e.what() << std::endl;
}
```

There is a syntax error in the JSON string. Running the above code prints:
```
parse error at offset 21: Missing a name for object member.
```

Take another example with a mismatch for the property value type:
```cpp
try {
    parse<Example>("{\"bool_value\": true, \"string_value\": []}");
} catch (JsonParseError& e) {
    std::cout << e.what() << std::endl;
}
```

Even though the above JSON string is syntactically correct, a JSON array cannot be cast into a C++ string, which produces an error:
```
parse error at offset 38: Terminate parsing due to Handler error.
```

Similarly, the deserializer not only reports the nature of the error encountered but also its location with a [JSON Pointer](https://www.rfc-editor.org/rfc/rfc6901). Consider the following:
```cpp
try {
    deserialize<Example>(
        "{"
            "\"bool_value\": true,"
            "\"string_value\": \"lorem ipsum\","
            "\"string_list\": [\"a\",23]"
        "}"
    );
} catch (JsonDeserializationError& e) {
    std::cout << e.what() << std::endl;
}
```

Notice how the second element of `string_list` (a C++ `vector<string>`) has the wrong data type: a JSON number instead of a JSON string. The error message identifies the location: `/string_list` to select the property in the root object and `/1` to identify the second element in the array (with zero-based indexing):
```
wrong JSON data type; expected: string at /string_list/1
```

## Limitations and workarounds

JSON parser does not support back-references (`{"$ref": "/path/to/previous/occurrence"}`); parsing pointer-like types always creates a new instance. When back-references are present, read the JSON string into a JSON DOM with the utility function `string_to_document`, and then de-serialize the data from JSON DOM with `deserialize`.

Parsing and de-serializing raw pointers is not permitted due to lack of clarity around ownership. Use `unique_ptr` and `shared_ptr` instead. Writing and serializing raw pointers is allowed, the pointee object is written.

JSON parser does not support variant types. Instead, read the JSON string into a JSON DOM with the utility function `string_to_document`, and then de-serialize the data from JSON DOM with `deserialize`. JSON writer, serializer and de-serializer support variant types.
