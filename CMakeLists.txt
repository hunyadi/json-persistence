cmake_minimum_required(VERSION 3.14)

# set up search path to vcpkg package manager such that dependencies installed with `vcpkg install` can be found
if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message("VCPKG_ROOT found")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(json-persistence VERSION 0.1)

# compiler configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    # set warning level 4 and treat all warnings as errors
    add_compile_options(/W4 /WX /Zc:__cplusplus,rvalueCast,strictStrings)
else()
    # enable lots of warnings and treat all warnings as errors (except those originating from RapidJSON)
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wfatal-errors -Wno-ambiguous-reversed-operator -Wno-deprecated-declarations)
endif()

# locate RapidJSON
find_package(RapidJSON CONFIG REQUIRED)
add_compile_definitions(RAPIDJSON_HAS_STDSTRING)

# locate GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# test configuration
enable_testing()

# project configuration
add_executable(json-persistence-tests ${CMAKE_SOURCE_DIR}/test/main.cpp ${CMAKE_SOURCE_DIR}/test/detail.cpp)
target_include_directories(json-persistence-tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${RAPIDJSON_INCLUDE_DIRS})
target_link_libraries(json-persistence-tests GTest::gtest_main)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_result)
if(ipo_result)
    set_property(TARGET json-persistence-tests PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

include(GoogleTest)
gtest_discover_tests(json-persistence-tests)
