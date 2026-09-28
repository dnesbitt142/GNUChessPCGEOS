/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* GNU Chess PC/GEOS port, GPL-3.0-or-later. */
#ifndef GC_PORTTYPES_H
#define GC_PORTTYPES_H
#ifdef __WATCOMC__
typedef signed long gc_int;
typedef unsigned long gc_uint;
#else
#include <stdint.h>
typedef int32_t gc_int;
typedef uint32_t gc_uint;
#endif
#endif
