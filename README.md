# Type-safe JSON persistence for C++17

This header-only C++17 library provides type-safe serialization of C++ objects into JSON strings, and deserialization of JSON strings to C++ objects.

## Features

* Header-only.
* Compile-time correctness.
* (De-)serialize fundamental types `bool`, `short`, `int`, `long`, `float`, `double`, etc. to/from their respective JSON type.
* (De-)serialize `time_point<system_clock>` type as ISO 8601 date-time string in time zone UTC.
* (De-)serialize heterogeneous `tuple<T...>` to/from a JSON array.
* (De-)serialize container types `vector<T>`, `array<T, N>`, `set<T>`, etc. to/from a JSON array.
* (De-)serialize dictionary types `map<string, T>` and `unordered_map<string, T>` to/from a JSON object.
* Serialize variant types to their stored type. De-serialize variant types using the first matching type.
* (De-)serialize class types by enumerating their member variables.
* Omit object fields for missing value in `optional<T>`.

## Design goals

### Intuitive syntax

Define JSON serialization and de-serialization within class definition:

```cpp
struct Example
{
    bool bool_value = false;
    std::string string_value;
    std::vector<std::string> string_list;
    std::optional<int> optional_value;
    UserDefinedType custom_value;

    template <typename Archive>
    void persist(Archive& ar)
    {
        ar
            & MEMBER_VARIABLE(bool_value)
            & MEMBER_VARIABLE(string_value)
            & MEMBER_VARIABLE(string_list)
            & MEMBER_VARIABLE(optional_value)
            & MEMBER_VARIABLE(custom_value)
            ;
    }
};
```

### Ease of use

* Get started by including `<persistence/persistence.hpp>`, which includes all features.
* Selectively include headers such as `<persistence/serialize_object.hpp>` to tackle a specific use case.
* Use `<persistence/object.hpp>` to import helper macro `MEMBER_VARIABLE` only.

### Speed and efficiency

Built on top of [RapidJSON](https://rapidjson.org/).

### Platform-neutral

Uses standard C++17 features only (with the exception of RapidJSON engine). Compiles on Linux, MacOS and Windows.