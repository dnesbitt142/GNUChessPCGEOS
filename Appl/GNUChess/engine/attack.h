/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* attack.h

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


// attack.h

#ifndef ATTACK_H
#define ATTACK_H

// includes

#include "board.h"
#include "util.h"
#include "vector.h"

// macros

#define IS_IN_CHECK(board,colour)         (is_attacked((board),KING_POS((board),(colour)),COLOUR_OPP((colour))))

#define DELTA_INC_LINE(delta)             (DeltaIncLine[DeltaOffset+(delta)])
#define DELTA_INC_ALL(delta)              (DeltaIncAll[DeltaOffset+(delta)])
#define DELTA_MASK(delta)                 (DeltaMask[DeltaOffset+(delta)])

#define INC_MASK(inc)                     (IncMask[IncOffset+(inc)])

#define PIECE_ATTACK(board,piece,from,to) (PSEUDO_ATTACK((piece),(to)-(from))&&line_is_empty((board),(from),(to)))
#define PSEUDO_ATTACK(piece,delta)        (((piece)&DELTA_MASK(delta))!=0)
#define SLIDER_ATTACK(piece,inc)          (((piece)&INC_MASK(inc))!=0)

#define ATTACK_IN_CHECK(attack)           ((attack)->dn!=0)

namespace engine {

// types

struct attack_t {
   gc_int dn;
   gc_int ds[2+1];
   gc_int di[2+1];
};

// variables

extern gc_int DeltaIncLine[DeltaNb];
extern gc_int DeltaIncAll[DeltaNb];

extern gc_int DeltaMask[DeltaNb];
extern gc_int IncMask[IncNb];

// functions

extern void attack_init   ();

extern bool is_attacked   (const board_t * board, gc_int to, gc_int colour);

extern bool line_is_empty (const board_t * board, gc_int from, gc_int to);

extern bool is_pinned     (const board_t * board, gc_int square, gc_int colour);

extern bool attack_is_ok  (const attack_t * attack);
extern void attack_set    (attack_t * attack, const board_t * board);

extern bool piece_attack_king (const board_t * board, gc_int piece, gc_int from, gc_int king);

}  // namespace engine

#endif // !defined ATTACK_H

// end of attack.h

