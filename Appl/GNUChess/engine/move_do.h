/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* move_do.h

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


// move_do.h

#ifndef MOVE_DO_H
#define MOVE_DO_H

// includes

#include "board.h"
#include "util.h"

namespace engine {

// types

struct undo_t {

   bool capture;

   gc_int capture_square;
   gc_int capture_piece;
   gc_int capture_pos;

   gc_int pawn_pos;

   gc_int turn;
   gc_int flags;
   gc_int ep_square;
   gc_int ply_nb;

   gc_int cap_sq;

   gc_int opening;
   gc_int endgame;

   uint64 key;
   uint64 pawn_key;
   uint64 material_key;
};

// functions

extern void move_do_init   ();

extern void move_do        (board_t * board, gc_int move, undo_t * undo);
extern void move_undo      (board_t * board, gc_int move, const undo_t * undo);

extern void move_do_null   (board_t * board, undo_t * undo);
extern void move_undo_null (board_t * board, const undo_t * undo);

}  // namespace engine

#endif // !defined MOVE_DO_H

// end of move_do.h

