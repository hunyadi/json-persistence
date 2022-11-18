#pragma once
#include <gtest/gtest-assertion-result.h>
#include "persistence/parse.hpp"
#include "persistence/deserialize.hpp"
#include "persistence/utility.hpp"

namespace test
{
    /**
     * De-serializes an object using various methods, and compares the result to a reference value.
     *
     * @param obj The JSON string to de-serialize.
     * @param ref_json The reference C++ object to compare against.
     */
    template<typename T>
    testing::AssertionResult test_deserialize(const std::string& str, const T& ref_obj)
    {
        using namespace ::persistence;

        bool result;

        rapidjson::Document doc = string_to_document(str);
        if (doc.HasParseError()) {
            return testing::AssertionFailure() << "document parse error";
        }

        T doc_obj;
        result = deserialize(doc, doc_obj);
        if (result) {
            if constexpr (is_pointer_like_v<T>) {
                EXPECT_EQ(*doc_obj, *ref_obj);
            } else {
                EXPECT_EQ(doc_obj, ref_obj);
            }
            doc_obj = deserialize<T>(doc);
            if constexpr (is_pointer_like_v<T>) {
                EXPECT_EQ(*doc_obj, *ref_obj);
                result = (*doc_obj == *ref_obj);
            } else {
                EXPECT_EQ(doc_obj, ref_obj);
                result = (doc_obj == ref_obj);
            }
        }
        if (!result) {
            return testing::AssertionFailure() << "de-serialization failed";
        }

        T str_obj;
        result = parse(str, str_obj);
        if (result) {
            if constexpr (is_pointer_like_v<T>) {
                EXPECT_EQ(*str_obj, *ref_obj);
            } else {
                EXPECT_EQ(str_obj, ref_obj);
            }
            str_obj = parse<T>(str);
            if constexpr (is_pointer_like_v<T>) {
                EXPECT_EQ(*str_obj, *ref_obj);
                result = (*str_obj == *ref_obj);
            } else {
                EXPECT_EQ(str_obj, ref_obj);
                result = (str_obj == ref_obj);
            }
        }
        if (!result) {
            return testing::AssertionFailure() << "parse from JSON failed";
        }

        return testing::AssertionSuccess();
    }

    template<typename T>
    testing::AssertionResult test_deserialize(const std::string& str, const T& ref_obj, bool& outcome)
    {
        auto result = test_deserialize(str, ref_obj);
        outcome = outcome && !!result;
        return result;
    }

    template<typename T>
    testing::AssertionResult test_no_deserialize(const std::string& str)
    {
        using namespace ::persistence;

        rapidjson::Document doc = string_to_document(str);
        if (doc.HasParseError()) {
            return testing::AssertionFailure() << "document parse error";
        }

        T obj;
        if (deserialize<T>(doc, obj)) {
            return testing::AssertionFailure() << "de-serialization succeeded unexpectedly";
        }
        EXPECT_THROW(deserialize<T>(doc), JsonDeserializationError);

        if (parse<T>(str, obj)) {
            return testing::AssertionFailure() << "parsing from JSON succeeded unexpectedly";
        }
        EXPECT_THROW(parse<T>(str), JsonParseError);

        return testing::AssertionSuccess();
    }

    template<typename T>
    testing::AssertionResult test_no_deserialize(const std::string& str, bool& outcome)
    {
        auto result = test_no_deserialize<T>(str);
        outcome = outcome && !!result;
        return result;
    }
}
