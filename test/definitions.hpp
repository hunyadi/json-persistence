#pragma once
#include "persistence/object.hpp"
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

struct TestValue
{
    TestValue() = default;
    TestValue(const TestValue&) = delete;
    TestValue(TestValue&&) = default;
    TestValue(const std::string& value) : value(value) {}

    template <typename Archive>
    Archive& persist(Archive& ar)
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
    void persist(Archive& ar)
    {
        ar
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
    void persist(Archive& ar)
    {
        ar
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
    void persist(Archive& ar)
    {
        ar
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
    void persist(Archive& ar)
    {
        ar
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
    void persist(Archive& ar)
    {
        ar
            & MEMBER_VARIABLE(values)
            ;
    }
};

struct TestBackReferenceObject
{
    std::shared_ptr<TestValue> outer;
    std::vector<std::shared_ptr<TestValue>> inner;

    template <typename Archive>
    void persist(Archive& ar)
    {
        ar
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
    void persist(Archive& ar)
    {
        ar
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
