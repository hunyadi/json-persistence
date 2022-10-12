#pragma once
#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson
{
    typedef ::std::size_t SizeType;
}
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
