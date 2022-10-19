#pragma once
#include "detail/config.hpp"
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <string>

namespace persistence
{
    /**
     * Generates a JSON string representation from a DOM object.
     */
    std::string document_to_string(const rapidjson::Document& doc)
    {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        return std::string(buffer.GetString(), buffer.GetSize());
    }

    /**
     * Parses a JSON string representation into a DOM object.
     */
    rapidjson::Document string_to_document(const std::string& str)
    {
        rapidjson::Document doc;
        doc.Parse(str.data());
        return doc;
    }

    /**
     * Parses a JSON string representation into a DOM object.
     */
    rapidjson::Document string_to_document(std::string&& str)
    {
        rapidjson::Document doc;
        doc.ParseInsitu(str.data());
        return doc;
    }
}
