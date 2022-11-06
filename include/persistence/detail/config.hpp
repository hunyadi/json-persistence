#pragma once

// detect instructions which speed up RapidJSON whitespace identification
#if defined(__SSE4_2__)
#define RAPIDJSON_SSE42
#elif defined(__SSE2__) || (defined(_MSC_VER) && defined(_M_X64))
#define RAPIDJSON_SSE2
#elif defined(__ARM_NEON)
#define RAPIDJSON_NEON
#endif
