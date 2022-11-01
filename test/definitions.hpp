#pragma once
#include "persistence/object.hpp"
#include "persistence/enum.hpp"
#include <optional>
#include <string>
#include <vector>

enum class Side : short
{
    Left = 1,
    Right = 2
};

enum class Suit
{
    Diamonds,
    Hearts,
    Clubs,
    Spades
};

enum class Esper { Unu, Du, Tri, Kvar, Kvin, Ses, Sep, Ok, Naux, Dek };

template<typename Key, typename Value>
std::map<Value, Key> flip(const std::map<Key, Value>& source)
{
    std::map<Value, Key> target;
    for (auto&& item : source) {
        target.insert(std::make_pair(item.second, item.first));
    }
    return target;
}

namespace persistence
{
    template<>
    struct enum_traits<Esper>
    {
        inline static const std::map<Esper, std::string_view> value_to_name = {
            std::make_pair(Esper::Unu, "Unu"),
            std::make_pair(Esper::Du, "Du"),
            std::make_pair(Esper::Tri, "Tri"),
            std::make_pair(Esper::Kvar, "Kvar"),
            std::make_pair(Esper::Kvin, "Kvin"),
            std::make_pair(Esper::Ses, "Ses"),
            std::make_pair(Esper::Sep, "Sep"),
            std::make_pair(Esper::Ok, "Ok"),
            std::make_pair(Esper::Naux, "Naux"),
            std::make_pair(Esper::Dek, "Dek")
        };

        inline static const std::map<std::string_view, Esper> name_to_value = flip(value_to_name);

        static std::string_view to_string(Esper value)
        {
            auto it = value_to_name.find(value);
            if (it != value_to_name.end()) {
                return it->second;
            } else {
                return std::string_view();
            }
        }

        static bool from_string(const std::string_view& name, Esper& value)
        {
            auto it = name_to_value.find(name);
            if (it != name_to_value.end()) {
                value = it->second;
                return true;
            } else {
                return false;
            }
        }
    };
}

struct TestValue
{
    TestValue() = default;
    TestValue(const TestValue&) = delete;
    TestValue(TestValue&&) = default;
    TestValue(const std::string& value) : value(value) {}

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(value)
            ;
    }

    bool operator==(const TestValue& op) const
    {
        return value == op.value;
    }

private:
    std::string value;
};

struct TestNonCopyable
{
    TestNonCopyable() = default;
    TestNonCopyable(const std::string& value) : member(value) {}

    TestValue member;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(member)
            ;
    }

    bool operator==(const TestNonCopyable& op) const
    {
        return member == op.member;
    }
};

class TestDataTransferSubObject
{
public:
    std::string member_value;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(member_value)
            ;
    }

    bool operator==(const TestDataTransferSubObject& op) const
    {
        return member_value == op.member_value;
    }
};

class TestDataTransferObject
{
public:
    bool bool_value = false;
    std::vector<bool> bool_list;
    int int_value = 0;
    std::vector<int> int_list;
    std::string string_value;
    std::vector<std::string> string_list;
    TestDataTransferSubObject object_value;
    std::vector<TestDataTransferSubObject> object_list;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(bool_value)
            & MEMBER_VARIABLE(bool_list)
            & MEMBER_VARIABLE(int_value)
            & MEMBER_VARIABLE(int_list)
            & MEMBER_VARIABLE(string_value)
            & MEMBER_VARIABLE(string_list)
            & MEMBER_VARIABLE(object_value)
            & MEMBER_VARIABLE(object_list)
            ;
    }

    bool operator==(const TestDataTransferObject& op) const
    {
        return bool_value == op.bool_value
            && bool_list == op.bool_list
            && int_value == op.int_value
            && int_list == op.int_list
            && string_value == op.string_value
            && string_list == op.string_list
            && object_value == op.object_value
            && object_list == op.object_list
        ;
    }
};

struct TestOptionalObjectMember
{
    TestOptionalObjectMember() = default;
    TestOptionalObjectMember(int value) : optional_value(value) {}

    std::optional<int> optional_value;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(optional_value)
            ;
    }

    bool operator==(const TestOptionalObjectMember& op) const
    {
        return optional_value == op.optional_value;
    }
};

struct TestBackReferenceArray
{
    std::vector<std::shared_ptr<TestValue>> values;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(values)
            ;
    }
};

struct TestBackReferenceObject
{
    std::shared_ptr<TestValue> outer;
    std::vector<std::shared_ptr<TestValue>> inner;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(outer)
            & MEMBER_VARIABLE(inner)
            ;
    }
};

/** Documentation example. */
struct UserDefinedType
{
    std::string value;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(value)
            ;
    }
};

/** Documentation example. */
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
