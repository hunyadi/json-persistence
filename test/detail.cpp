#include <gtest/gtest.h>
#include "persistence/detail/polymorphic_stack.hpp"
#include "persistence/base64.hpp"
#include "persistence/object_members.hpp"
#include "capture.hpp"
#include "definitions.hpp"

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
    Example cls;
    constexpr auto members = member_variables(cls);
    EXPECT_EQ(std::get<0>(members).name, "bool_value");
    EXPECT_EQ(std::get<1>(members).name, "string_value");
    EXPECT_EQ(std::get<2>(members).name, "string_list");
    EXPECT_EQ(std::get<3>(members).name, "optional_value");
    EXPECT_EQ(std::get<4>(members).name, "custom_value");
}

void test_base64(std::string_view str, std::string_view ref)
{
    std::string out;
    base64_encode(str, out);
    EXPECT_EQ(out, ref);

    byte_vector raw;
    base64_decode(ref, raw);
    std::string_view val(
        reinterpret_cast<const char*>(raw.data()),
        raw.size()
    );
    EXPECT_EQ(val, str);
}

void test_no_base64_decode(std::string_view ref)
{
    byte_vector raw;
    EXPECT_FALSE(base64_decode(ref, raw));
}

TEST(Utility, Base64)
{
   test_base64("", "");
   test_base64("f", "Zg==");
   test_base64("fo", "Zm8=");
   test_base64("foo", "Zm9v");
   test_base64("foob", "Zm9vYg==");
   test_base64("fooba", "Zm9vYmE=");
   test_base64("foobar", "Zm9vYmFy");
   test_base64("Hello World", "SGVsbG8gV29ybGQ=");
   test_base64("The quick brown fox jumped over the lazy dogs.", "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wZWQgb3ZlciB0aGUgbGF6eSBkb2dzLg==");

   test_no_base64_decode("A");
   test_no_base64_decode("AA");
   test_no_base64_decode("AAA");
   test_no_base64_decode("A===");
   test_no_base64_decode("===A");
   test_no_base64_decode("==AA");
   test_no_base64_decode("=AAA");
   test_no_base64_decode("AAA.");
   test_no_base64_decode(".AAA");
   test_no_base64_decode("OOOO==AA");
   test_no_base64_decode("OOOO=AAA");
   test_no_base64_decode("OOOOAAA.");
   test_no_base64_decode("OOOO.AAA");
}
