#include <gtest/gtest.h>
#include "persistence/detail/polymorphic_stack.hpp"
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
