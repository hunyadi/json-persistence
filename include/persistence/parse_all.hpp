#pragma once
/**
* Parser functions instantiate a C++ object based on a JSON string.
*/

#include "detail/version.hpp"
#include "parse.hpp"
#include "parse_fundamental.hpp"
#include "parse_object.hpp"
#include "parse_pointer.hpp"
#include "parse_array.hpp"
#include "parse_bytes.hpp"
#include "parse_datetime.hpp"
#include "parse_enum.hpp"
#include "parse_map.hpp"
#include "parse_set.hpp"
#include "parse_string.hpp"
#include "parse_vector.hpp"
#if __cplusplus >= 202002L
#include "parse_date.hpp"
#endif
