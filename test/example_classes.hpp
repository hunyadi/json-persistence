#pragma once
#include "persistence/object.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct TestValue
{
    TestValue() = default;
    TestValue(const std::string& value) : value(value) {}
    TestValue(const TestValue&) = delete;
    TestValue(TestValue&&) = default;
    TestValue& operator=(const TestValue&) = default;

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

    bool operator<(const TestValue& op) const
    {
        return value < op.value;
    }

private:
    std::string value;
};

struct TestDefault
{
    TestDefault() = default;
    TestDefault(const std::string& value)
        : value(value)
    {}

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE_DEFAULT(value)
            ;
    }

    bool operator==(const TestDefault& op) const
    {
        return value == op.value;
    }

    bool operator<(const TestDefault& op) const
    {
        return value < op.value;
    }

private:
    std::string value = "default";
};

struct TestDerived : TestValue
{
    TestDerived() = default;
    TestDerived(const std::string& value, const std::string& member)
        : TestValue(value)
        , member(member)
    {}

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return this->TestValue::persist(ar)
            & MEMBER_VARIABLE(member)
            ;
    }

    bool operator==(const TestDerived& op) const
    {
        return this->TestValue::operator==(op) && member == op.member;
    }

private:
    std::string member;
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

struct TestPair
{
    int first = 1;
    int second = 2;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(first)
            & MEMBER_VARIABLE(second)
            ;
    }

    bool operator==(const TestPair& op) const
    {
        return first == op.first && second == op.second;
    }
};

class TestDataTransferSubObject
{
public:
    std::string value;

    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & NAMED_MEMBER_VARIABLE("member_value", value)
            ;
    }

    bool operator==(const TestDataTransferSubObject& op) const
    {
        return value == op.value;
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

/** Documentation example. */
struct Base
{
    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return ar
            & MEMBER_VARIABLE(base_member)
            ;
    }

private:
    std::string base_member = "base";
};

/** Documentation example. */
struct Derived : Base
{
    template <typename Archive>
    constexpr auto persist(Archive& ar)
    {
        return this->Base::persist(ar)
            & MEMBER_VARIABLE(derived_member)
            ;
    }

private:
    std::string derived_member = "derived";
};
