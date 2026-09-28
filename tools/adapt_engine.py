#!/usr/bin/env python3
"""Generate the constrained-memory engine from GNU Chess 6.3.0 sources."""
import re,sys
from pathlib import Path
src=Path(sys.argv[1]); out=Path(sys.argv[2]); out.mkdir(parents=True,exist_ok=True)
exclude={'main','protocol','posix','book','util','search'}
# Preserve comments and literals; retain 32-bit arithmetic on 16-bit Watcom.
lex=re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|\bint\b',re.S)
for p in src.iterdir():
 if p.suffix not in ('.cpp','.h') or p.stem in exclude:continue
 s=p.read_text()
 s=lex.sub(lambda m:'gc_int' if m.group()=='int' else m.group(),s)
 s=s.replace('#include <cstdlib>','#include "portlib.h"').replace('#include <cstring>','#include "portlib.h"').replace('#include <cstdio>','#include "portlib.h"').replace('#include <cctype>','#include "portlib.h"')
 s=s.replace('const gc_int StackSize = 4096;', 'const gc_int StackSize = 256; // GEOS: bounded repetition history')
 # Avoid 16-bit compile-time overflow before assignment to gc_int.
 s=re.sub(r'\b(0[xX][0-9a-fA-F]+|[0-9]+)\s*<<',r'\1L <<',s)
 # Arrays use short only where entries are provably within [-32768,32767].
 if p.name=='attack.cpp':
  s=s.replace('gc_int PieceDeltaSize[4][256]','sint16 PieceDeltaSize[4][256]').replace('gc_int PieceDeltaDelta[4][256][4]','sint16 PieceDeltaDelta[4][256][4]')
  s=s.replace('const gc_int * delta_ptr;', 'const sint16 * delta_ptr;')
 if p.name=='pawn.cpp':s=s.replace('TableSize = 16384; // 256kB','TableSize = 512; // GEOS: 8 KiB')
 if p.name=='trans.cpp':
  start=s.index('   target = option_get_int("Hash");');end=s.index('   // allocate table',start)
  s=s[:start]+'''   // GEOS: 1,024 16-byte entries plus three cluster spill entries.
   // Each allocation must remain below 64 KiB.
   target = 16384UL;
   size = target;

'''+s[end:]
  start=s.index('void trans_stats('); end=s.index('// trans_entry()',start)
  s=s[:start]+'''void trans_stats(const trans_t * trans) {
   // Native UI does not emit UCI statistics or use floating-point arithmetic.
   (void)trans;
}

'''+s[end:]
  s=s.replace('sint64 read_', 'uint32 read_').replace('sint64 write_', 'uint32 write_')
 if p.name=='option.cpp':
  s=s.replace('#include "config.h"','')
  start=s.index('   // Add default book path');end=s.index('// option_list()',start)
  s=s[:start]+'}\n\n'+s[end:]
  start=s.index('static void get_default_book_file_path(');end=s.index('\n',start)
  s=s[:start]+s[end:]
  start=s.index('// get_default_book_file_path');end=s.index('}  // namespace engine',start)
  s=s[:start]+s[end:]
 if p.name=='hash.h':
  s=s.replace('uint32((key)>>32)', 'engine::high32(key)')
 if p.name=='random.cpp':
  s=s.replace('(Random64[RandomNb-1] >> 32)', 'high32(Random64[RandomNb-1])')
 if p.name=='eval.cpp':
  s=s.replace('ASSERT(!board_is_check(board)); // exceptions are extremely rare', '// GEOS: a checked position may be statically evaluated at the hard height cap.')
 if p.name=='fen.cpp':s=s.replace('sprintf(&fen[pos],"%d 1",board->ply_nb);','gc_format_ply(&fen[pos],board->ply_nb);')
 # Prefix all files before their own header to define gc_int.
 s='/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */\n#include "porttypes.h"\n'+s
 (out/p.name).write_text(s)
