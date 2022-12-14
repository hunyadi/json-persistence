#pragma once
#include <chrono>
#include <iostream>
#include <type_traits>

namespace test
{
    /** True if a nullary function has a return type other than `void`. */
    template<typename, typename = void>
    struct returns_nonvoid : std::false_type {};

    template<typename F>
    struct returns_nonvoid<F, std::enable_if_t<!std::is_void_v<decltype(std::declval<F>()())>>> : std::true_type {};

    /** True if a nullary function has a return type of `void`. */
    template<typename, typename = void>
    struct returns_void : std::false_type {};

    template<typename F>
    struct returns_void<F, std::enable_if_t<std::is_void_v<decltype(std::declval<F>()())>>> : std::true_type {};

    /** Measures time elapsed by executing a code block. */
    struct Timer
    {
        using time_point = std::chrono::high_resolution_clock::time_point;

        Timer(const char* description)
            : description(description)
        {}

        ~Timer()
        {
            stop();
        }

        void start()
        {
            started_at = std::chrono::high_resolution_clock::now();
        }

        void stop()
        {
            if (started_at != time_point()) {
                auto finished_at = std::chrono::high_resolution_clock::now();
                std::cout << description << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(finished_at - started_at).count() << " ms" << std::endl;
                started_at = time_point();
            }
        }

    private:
        const char* description;
        time_point started_at;
    };

    template<typename F, std::enable_if_t<returns_nonvoid<F>::value, int> = 0>
    auto measure(const char* description, F operation)
    {
        Timer timer(description);
        timer.start();
        auto result = operation();
        timer.stop();
        return result;
    }

    template<typename F, std::enable_if_t<returns_void<F>::value, int> = 0>
    void measure(const char* description, F operation)
    {
        Timer timer(description);
        timer.start();
        operation();
        timer.stop();
    }
}
