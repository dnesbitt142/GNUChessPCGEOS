/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* value.h

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


// value.h

#ifndef VALUE_H
#define VALUE_H

// includes

#include "piece.h"
#include "util.h"

namespace engine {

// constants

const gc_int ValuePawn   = 100;   // was 100
const gc_int ValueKnight = 325;   // was 300
const gc_int ValueBishop = 325;   // was 300
const gc_int ValueRook   = 500;   // was 500
const gc_int ValueQueen  = 1000;  // was 900
const gc_int ValueKing   = 10000; // was 10000

const gc_int ValueNone    = -32767;
const gc_int ValueDraw    = 0;
const gc_int ValueMate    = 30000;
const gc_int ValueInf     = ValueMate;
const gc_int ValueEvalInf = ValueMate - 256; // handle mates upto 255 plies

// macros

#define VALUE_MATE(height) (-ValueMate+(height))
#define VALUE_PIECE(piece) (ValuePiece[piece])

// variables

extern gc_int ValuePiece[PieceNb];

// functions

extern void value_init       ();

extern bool value_is_ok      (gc_int value);
extern bool range_is_ok      (gc_int min, gc_int max);

extern bool value_is_mate    (gc_int value);

extern gc_int  value_to_trans   (gc_int value, gc_int height);
extern gc_int  value_from_trans (gc_int value, gc_int height);

extern gc_int  value_to_mate    (gc_int value);

}  // namespace engine

#endif // !defined VALUE_H

// end of value.h

