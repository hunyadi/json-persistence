cmake_minimum_required(VERSION 3.14)

# set up search path to vcpkg package manager such that dependencies installed with `vcpkg install` can be found
if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message("VCPKG_ROOT found")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(json-persistence VERSION 0.1)

# locate dependency: RapidJSON
find_package(RapidJSON CONFIG REQUIRED)

# locate test dependency: Boost (optional)
find_package(Boost)
if(Boost_FOUND)
    if(NOT MSVC)
        add_compile_options(-Wno-gnu-zero-variadic-macro-arguments)
    endif()
endif()

# locate test dependency: GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

# library target configuration
file(GLOB JSON_LIBRARY_SOURCES
    ${CMAKE_SOURCE_DIR}/test/*.cpp
)
file(GLOB JSON_LIBRARY_HEADERS
    ${CMAKE_SOURCE_DIR}/include/persistence/*.hpp
    ${CMAKE_SOURCE_DIR}/include/persistence/detail/*.hpp
)
add_library(json-persistence INTERFACE)

# compiler options
target_compile_features(json-persistence INTERFACE cxx_std_17)
set_property(TARGET json-persistence PROPERTY VISIBILITY_INLINES_HIDDEN ON)
set_property(TARGET json-persistence PROPERTY PUBLIC_HEADER ${JSON_LIBRARY_HEADERS})

if(MSVC)
    target_compile_definitions(json-persistence INTERFACE _CRT_SECURE_NO_WARNINGS)
    # set warning level 4 and treat all warnings as errors
    target_compile_options(json-persistence INTERFACE /permissive- /W4 /WX /Zc:__cplusplus,enumTypes,lambda,referenceBinding,rvalueCast,strictStrings,ternary)
else()
    # enable lots of warnings and treat all warnings as errors (except those originating from RapidJSON)
    target_compile_options(json-persistence INTERFACE -Wall -Wextra -pedantic -Werror -Wfatal-errors -Wno-ambiguous-reversed-operator -Wno-deprecated-declarations)
endif()

# include directories
target_include_directories(json-persistence INTERFACE ${CMAKE_SOURCE_DIR}/include ${RAPIDJSON_INCLUDE_DIRS})
if(Boost_FOUND)
    target_compile_definitions(json-persistence INTERFACE PERSISTENCE_BOOST_DESCRIBE)
    target_include_directories(json-persistence INTERFACE ${Boost_INCLUDE_DIRS})
endif()

# build configuration
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_result)
if(ipo_result)
    set_property(TARGET json-persistence PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# test target configuration
add_executable(json-persistence-tests ${JSON_LIBRARY_SOURCES})
target_link_libraries(json-persistence-tests json-persistence GTest::gtest_main)

# test configuration
include(GoogleTest)
gtest_discover_tests(json-persistence-tests)

# install configuration
include(GNUInstallDirs)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/persistence
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
