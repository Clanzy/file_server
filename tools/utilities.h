#pragma once

/** 
 * This macro is borrowed from 
 * https://stackoverflow.com/questions/3022552/is-there-any-standard-htonl-like-function-for-64-bits-integers-in-c 
 */
#include <arpa/inet.h>

#if __BIG_ENDIAN__
#define htonll(x) (x)
#define ntohll(x) (x)
#else
#define htonll(x) (((uint64_t)htonl((x)&0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) (((uint64_t)ntohl((x)&0xFFFFFFFF) << 32) | ntohl((x) >> 32))
#endif

/** 
 * This macro is borrowed from 
 * https://stackoverflow.com/questions/3437404/min-and-max-in-c
 */
#define max(a, b)                       \
	({                              \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a > _b ? _a : _b;      \
	})

#define min(a, b)                       \
	({                              \
		__typeof__(a) _a = (a); \
		__typeof__(b) _b = (b); \
		_a < _b ? _a : _b;      \
	})
/**
 * Foreach macro that iterates through tokens of string
 * Taken from https://github.com/torvalds/linux/blob/master/tools/perf/builtin-c2c.c
 */
#define for_each_token(__tok, __buf, __sep, __tmp)          \
	for (__tok = strtok_r(__buf, __sep, &__tmp); __tok; \
	     __tok = strtok_r(NULL, __sep, &__tmp))
