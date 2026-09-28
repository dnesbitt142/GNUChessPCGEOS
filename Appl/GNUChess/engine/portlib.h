/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* Native C runtime bridge; do not include Watcom's DOS C++ runtime. */
#ifndef GC_PORTLIB_H
#define GC_PORTLIB_H
#include "porttypes.h"
#ifdef __WATCOMC__
extern "C" {
#include <geos.h>
#include <Ansi/string.h>
#include <Ansi/stdlib.h>
#include <Ansi/stdio.h>
#include <Ansi/ctype.h>
}
#else
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#endif
inline gc_int gc_abs(gc_int n) { return n < 0 ? -n : n; }
#define abs gc_abs
void gc_format_ply(char *p, gc_int n);
#endif
