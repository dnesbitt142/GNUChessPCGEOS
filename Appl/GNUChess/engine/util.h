/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* GNU Chess PC/GEOS portability layer. GPL-3.0-or-later. */
#ifndef UTIL_H
#define UTIL_H
#include "porttypes.h"
#include "portlib.h"
#undef TRUE
#undef FALSE
#define TRUE 1
#define FALSE 0
#ifdef GC_DEBUG
#define DEBUG 1
#define ASSERT(a) do { if (!(a)) engine::my_fatal("%s:%d: %s",__FILE__,__LINE__,#a); } while (0)
#else
#define DEBUG 0
#define ASSERT(a)
#endif
#define U64(n) n##ULL
#define S64(n) n##LL
#define S64_FORMAT "%lld"
#define U64_FORMAT "%016llX"
namespace engine {
typedef signed char sint8;
typedef unsigned char uint8;
typedef signed short sint16;
typedef unsigned short uint16;
typedef gc_int sint32;
typedef gc_uint uint32;
typedef char verify_gc_int_is_32_bits[(sizeof(gc_int)==4)?1:-1];
typedef char verify_short_is_16_bits[(sizeof(short)==2)?1:-1];
#ifdef __WATCOMC__
typedef signed __int64 sint64;
typedef unsigned __int64 uint64;
#else
typedef int64_t sint64;
typedef uint64_t uint64;
#endif
inline uint32 high32(uint64 key) {
#ifdef __WATCOMC__
 // GEOS is little endian. Avoid an unavailable Watcom __U8RS helper.
 uint32 hi; memcpy(&hi, ((const char *)&key)+4, sizeof(hi)); return hi;
#else
 return (uint32)(key >> 32);
#endif
}
extern bool pool_init();
extern gc_int pool_used();
extern void *my_malloc(gc_int size);
extern void my_free(void *p);
extern void my_fatal(const char *format,...);
extern bool my_string_empty(const char *s);
extern bool my_string_equal(const char *a,const char *b);
extern char *my_strdup(const char *s);
extern void my_string_clear(const char **p);
extern void my_string_set(const char **p,const char *s);
}
#endif
