/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* move.h

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


// move.h

#ifndef MOVE_H
#define MOVE_H

// includes

#include "board.h"
#include "util.h"

namespace engine {

// constants

const gc_int MoveNone = 0;  // HACK: a1a1 cannot be a legal move
const gc_int MoveNull = 11; // HACK: a1d2 cannot be a legal move

const gc_int MoveNormal    = 0L << 14;
const gc_int MoveCastle    = 1L << 14;
const gc_int MovePromote   = 2L << 14;
const gc_int MoveEnPassant = 3L << 14;
const gc_int MoveFlags     = 3L << 14;

const gc_int MovePromoteKnight = MovePromote | (0L << 12);
const gc_int MovePromoteBishop = MovePromote | (1L << 12);
const gc_int MovePromoteRook   = MovePromote | (2L << 12);
const gc_int MovePromoteQueen  = MovePromote | (3L << 12);

const gc_int MoveAllFlags = 0xFL << 12;

const char NullMoveString[] = "null"; // "0000" in UCI

// macros

#define MOVE_MAKE(from,to)             ((SQUARE_TO_64(from)<<6)|SQUARE_TO_64(to))
#define MOVE_MAKE_FLAGS(from,to,flags) ((SQUARE_TO_64(from)<<6)|SQUARE_TO_64(to)|(flags))

#define MOVE_FROM(move)                (SQUARE_FROM_64(((move)>>6)&077))
#define MOVE_TO(move)                  (SQUARE_FROM_64((move)&077))

#define MOVE_IS_SPECIAL(move)          (((move)&MoveFlags)!=MoveNormal)
#define MOVE_IS_PROMOTE(move)          (((move)&MoveFlags)==MovePromote)
#define MOVE_IS_EN_PASSANT(move)       (((move)&MoveFlags)==MoveEnPassant)
#define MOVE_IS_CASTLE(move)           (((move)&MoveFlags)==MoveCastle)

#define MOVE_PIECE(move,board)         ((board)->square[MOVE_FROM(move)])

// types

typedef uint16 mv_t;

// functions

extern bool move_is_ok            (gc_int move);

extern gc_int  move_promote          (gc_int move);

extern gc_int  move_order            (gc_int move);

extern bool move_is_capture       (gc_int move, const board_t * board);
extern bool move_is_under_promote (gc_int move);
extern bool move_is_tactical      (gc_int move, const board_t * board);

extern gc_int  move_capture          (gc_int move, const board_t * board);

extern bool move_to_string        (gc_int move, char string[], gc_int size);
extern gc_int  move_from_string      (const char string[], const board_t * board);

}  // namespace engine

#endif // !defined MOVE_H

// end of move.h

