#pragma once
/**
* Deserialization functions instantiate a C++ object based on values in a JSON DOM object.
*/

#include "deserialize.hpp"
#include "deserialize_fundamental.hpp"
#include "deserialize_object.hpp"
#include "deserialize_pointer.hpp"
#include "deserialize_array.hpp"
#include "deserialize_bytes.hpp"
#include "deserialize_datetime.hpp"
#include "deserialize_enum.hpp"
#include "deserialize_map.hpp"
#include "deserialize_set.hpp"
#include "deserialize_string.hpp"
#include "deserialize_tuple.hpp"
#include "deserialize_variant.hpp"
#include "deserialize_vector.hpp"
#if __cplusplus >= 202002L
#include "deserialize_date.hpp"
#endif
