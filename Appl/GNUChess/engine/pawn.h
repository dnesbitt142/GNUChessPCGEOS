/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* pawn.h

   GNU Chess engine

   Copyright (C) 2001-2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


// pawn.h

#ifndef PAWN_H
#define PAWN_H

// includes

#include "board.h"
#include "colour.h"
#include "util.h"

// macros

#define BIT(n)       ((gc_int)BitEQ[n])

#define BIT_FIRST(b) ((gc_int)BitFirst[b])
#define BIT_LAST(b)  ((gc_int)BitLast[b])
#define BIT_COUNT(b) ((gc_int)BitCount[b])

namespace engine {

// constants

const gc_int BackRankFlag = 1L << 0;

// types

struct pawn_info_t {
   uint32 lock;
   sint16 opening;
   sint16 endgame;
   uint8 flags[ColourNb];
   uint8 passed_bits[ColourNb];
   uint8 single_file[ColourNb];
   uint16 pad;
};

// variables

extern uint8 BitEQ[16];
extern uint8 BitLT[16];
extern uint8 BitLE[16];
extern uint8 BitGT[16];
extern uint8 BitGE[16];

extern uint8 BitFirst[0x100];
extern uint8 BitLast[0x100];
extern uint8 BitCount[0x100];
extern uint8 BitRev[0x100];

// functions

extern void pawn_init_bit ();
extern void pawn_init     ();

extern void pawn_alloc    ();
extern void pawn_clear    ();

extern void pawn_get_info (pawn_info_t * info, const board_t * board);

extern gc_int  quad          (gc_int y_min, gc_int y_max, gc_int x);

}  // namespace engine

#endif // !defined PAWN_H

// end of pawn.h

