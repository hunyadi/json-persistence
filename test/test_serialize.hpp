#pragma once
#include <gtest/gtest-assertion-result.h>
#include "persistence/write.hpp"
#include "persistence/serialize.hpp"
#include "persistence/utility.hpp"

namespace test
{
    /**
     * Serializes an object using various methods, and compares the result to a reference value.
     *
     * @param obj The C++ object to serialize.
     * @param ref_json The reference JSON string to compare against.
     */
    template<typename T>
    testing::AssertionResult test_serialize(const T& obj, const std::string& ref_json)
    {
        using namespace ::persistence;

        bool result;

        rapidjson::Document doc;
        result = serialize_to_document(obj, doc);
        if (result) {
            auto&& doc_json = document_to_string(doc);
            EXPECT_EQ(doc_json, ref_json);
            result = (doc_json == ref_json);
        }
        if (!result) {
            return testing::AssertionFailure() << "serialization failed";
        }

        std::string str_json;
        result = write_to_string(obj, str_json);
        if (result) {
            EXPECT_EQ(str_json, ref_json);
            EXPECT_EQ(write_to_string(obj), ref_json);
            result = (str_json == ref_json);
        }
        if (!result) {
            return testing::AssertionFailure() << "writing to JSON failed";
        }

        return testing::AssertionSuccess();
    }

    template<typename T>
    testing::AssertionResult test_serialize(const T& obj, const std::string& ref_json, bool& outcome)
    {
        auto result = test_serialize(obj, ref_json);
        outcome = outcome && !!result;
        return result;
    }
}
