#include <gtest/gtest.h>
#include "persistence/detail/polymorphic_stack.hpp"
#include "persistence/base64.hpp"
#include "persistence/object_members.hpp"
#include "example_classes.hpp"
#include "capture.hpp"
#include "measure.hpp"

using namespace persistence;
using namespace test;

struct BaseClass
{
    virtual void process() = 0;
    virtual ~BaseClass() {}
};

struct DerivedClassA : BaseClass
{
    DerivedClassA() = default;
    DerivedClassA(char c) : c(c) {}

    void process()
    {
        std::cout << "A " << c << ", ";
    }

    ~DerivedClassA()
    {
        std::cout << "~A, ";
    }

    char c = 'a';
};

struct DerivedClassB : BaseClass
{
    void process()
    {
        std::cout << "B, ";
    }

    ~DerivedClassB()
    {
        std::cout << "~B, ";
    }

    int i = 82;
    double d = 4.5;
};

TEST(Utility, PolymorphicStack)
{
    OutputCapture capture;
    []{
        detail::PolymorphicStack<BaseClass> stack;
        stack.emplace<DerivedClassA>();
        stack.back().process();
        stack.emplace<DerivedClassA>('b');
        stack.back().process();
        stack.emplace<DerivedClassB>();
        stack.back().process();
        stack.pop();
        stack.back().process();
        stack.emplace<DerivedClassA>('c');
        stack.back().process();
    }();
    EXPECT_EQ(capture.str(), "A a, A b, B, ~B, A b, A c, ~A, ~A, ~A, ");
}

TEST(Utility, Members)
{
    const Example object;
    constexpr auto members = member_variables(object);
    EXPECT_EQ(std::get<0>(members).name(), "bool_value");
    EXPECT_EQ(std::get<1>(members).name(), "string_value");
    EXPECT_EQ(std::get<2>(members).name(), "string_list");
    EXPECT_EQ(std::get<3>(members).name(), "optional_value");
    EXPECT_EQ(std::get<4>(members).name(), "custom_value");
}

testing::AssertionResult test_base64(const std::string_view& str, std::string_view ref)
{
    std::string out;
    if (!base64_encode(str, out)) {
        return testing::AssertionFailure() << "encode failed";
    }
    if (out != ref) {
        return testing::AssertionFailure() << "encode mismatch; expected: " << ref << ", got: " << out;
    }

    byte_vector raw;
    if (!base64_decode(ref, raw)) {
        return testing::AssertionFailure() << "decode failed";
    }
    std::string_view val(
        reinterpret_cast<const char*>(raw.data()),
        raw.size()
    );
    if (val != str) {
        return testing::AssertionFailure() << "decode mismatch";
    }
    return testing::AssertionSuccess();
}

testing::AssertionResult test_base64_encode(const byte_vector& vec)
{
    std::string out;
    if (base64_encode(vec, out)) {
        return testing::AssertionSuccess() << "encode successful";
    } else {
        return testing::AssertionFailure() << "encode failed";
    }
}

testing::AssertionResult test_base64_decode(const std::string_view& ref)
{
    byte_vector raw;
    if (base64_decode(ref, raw)) {
        return testing::AssertionSuccess() << "decode successful";
    } else {
        return testing::AssertionFailure() << "decode failed";
    }
}

TEST(Utility, Base64)
{
    EXPECT_TRUE(test_base64("", ""));
    EXPECT_TRUE(test_base64("f", "Zg=="));
    EXPECT_TRUE(test_base64("fo", "Zm8="));
    EXPECT_TRUE(test_base64("foo", "Zm9v"));
    EXPECT_TRUE(test_base64("foob", "Zm9vYg=="));
    EXPECT_TRUE(test_base64("fooba", "Zm9vYmE="));
    EXPECT_TRUE(test_base64("foobar", "Zm9vYmFy"));
    EXPECT_TRUE(test_base64("Hello World", "SGVsbG8gV29ybGQ="));
    EXPECT_TRUE(test_base64("The quick brown fox jumped over the lazy dogs.", "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBkb2dzLg=="));

    EXPECT_FALSE(test_base64_decode("A"));
    EXPECT_FALSE(test_base64_decode("AA"));
    EXPECT_FALSE(test_base64_decode("AAA"));
    EXPECT_FALSE(test_base64_decode("A==="));
    EXPECT_FALSE(test_base64_decode("===A"));
    EXPECT_FALSE(test_base64_decode("==AA"));
    EXPECT_FALSE(test_base64_decode("=AAA"));
    EXPECT_FALSE(test_base64_decode("AAA."));
    EXPECT_FALSE(test_base64_decode(".AAA"));
    EXPECT_FALSE(test_base64_decode("OOOO==AA"));
    EXPECT_FALSE(test_base64_decode("OOOO=AAA"));
    EXPECT_FALSE(test_base64_decode("OOOOAAA."));
    EXPECT_FALSE(test_base64_decode("OOOO.AAA"));
}

#ifndef _DEBUG
TEST(Performance, Base64)
{
    byte_vector vec;
    for (std::size_t k = 0; k < 1000000000; ++k) {
        vec.push_back(std::byte(k % 256));
    }
    std::string enc = measure("Base64 encode", [&]() {
        std::string out;
        base64_encode(vec, out);
        return out;
    });
    measure("Base64 decode", [&]() {
        byte_vector raw;
        base64_decode(enc, raw);
    });
}
#endif
