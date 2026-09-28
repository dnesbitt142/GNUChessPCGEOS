/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* vector.h

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


// vector.h

#ifndef VECTOR_H
#define VECTOR_H

// includes

#include "util.h"

namespace engine {

// "constants"

const gc_int IncNone = 0;
const gc_int IncNb = 2 * 17 + 1;
const gc_int IncOffset = 17;

const gc_int DeltaNone = 0;
const gc_int DeltaNb = 2 * 119 + 1;
const gc_int DeltaOffset = 119;

// macros

#define DISTANCE(square_1,square_2) (Distance[DeltaOffset+((square_2)-(square_1))])

// variables

extern gc_int Distance[DeltaNb];

// functions

extern void vector_init ();

extern bool delta_is_ok (gc_int delta);
extern bool inc_is_ok   (gc_int inc);

}  // namespace engine

#endif // !defined VECTOR_H

// end of vector.h

