#pragma once
/**
* Serialization functions convert a C++ object into values in a JSON DOM object.
*/

#include "detail/version.hpp"
#include "serialize.hpp"
#include "serialize_fundamental.hpp"
#include "serialize_object.hpp"
#include "serialize_pointer.hpp"
#include "serialize_array.hpp"
#include "serialize_bytes.hpp"
#include "serialize_datetime.hpp"
#include "serialize_enum.hpp"
#include "serialize_map.hpp"
#include "serialize_set.hpp"
#include "serialize_string.hpp"
#include "serialize_tuple.hpp"
#include "serialize_variant.hpp"
#include "serialize_vector.hpp"
#if __cplusplus >= 202002L
#include "serialize_date.hpp"
#endif
