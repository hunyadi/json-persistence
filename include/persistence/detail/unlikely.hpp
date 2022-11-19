#pragma once
#include "version.hpp"
#if (__cplusplus >= 202002L) && (__has_cpp_attribute(unlikely) >= 201803L)
#define PERSISTENCE_IF_LIKELY(condition) if (condition) [[likely]]
#define PERSISTENCE_IF_UNLIKELY(condition) if (condition) [[unlikely]]
#elif defined __GNUC__
#define PERSISTENCE_IF_LIKELY(condition) if (__builtin_expect(!!(condition), 1))
#define PERSISTENCE_IF_UNLIKELY(condition) if (__builtin_expect(!!(condition), 0))
#else
#define PERSISTENCE_IF_LIKELY(condition) if (condition)
#define PERSISTENCE_IF_UNLIKELY(condition) if (condition)
#endif
