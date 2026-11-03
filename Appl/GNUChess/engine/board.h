/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* board.h

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


// board.h

#ifndef BOARD_H
#define BOARD_H

// includes

#include "colour.h"
#include "piece.h"
#include "square.h"
#include "util.h"

namespace engine {

// constants

const gc_int Empty = 0;
const gc_int Edge = Knight64; // HACK: uncoloured knight

const gc_int WP = WhitePawn256;
const gc_int WN = WhiteKnight256;
const gc_int WB = WhiteBishop256;
const gc_int WR = WhiteRook256;
const gc_int WQ = WhiteQueen256;
const gc_int WK = WhiteKing256;

const gc_int BP = BlackPawn256;
const gc_int BN = BlackKnight256;
const gc_int BB = BlackBishop256;
const gc_int BR = BlackRook256;
const gc_int BQ = BlackQueen256;
const gc_int BK = BlackKing256;

const gc_int FlagsNone = 0;
const gc_int FlagsWhiteKingCastle  = 1L << 0;
const gc_int FlagsWhiteQueenCastle = 1L << 1;
const gc_int FlagsBlackKingCastle  = 1L << 2;
const gc_int FlagsBlackQueenCastle = 1L << 3;

const gc_int StackSize = 256; // GEOS: bounded repetition history

// macros

#define KING_POS(board,colour) ((board)->piece[colour][0])

// types

// Board storage only: legal mailbox squares are 0x44..0xbb and sentinel is 0.
// Keep sq_t/gc_int arithmetic and direction types unchanged.
typedef uint8 board_sq_t;
const gc_int BoardPieceCapacity = 16 + 1; // all original men + sentinel
const gc_int BoardPawnCapacity = 8 + 1;   // promotion never creates pawns

struct board_t {

   uint8 square[SquareNb]; // piece/edge codes are 0..255
   sint8 pos[SquareNb]; // -1 sentinel or list index 0..15

   board_sq_t piece[ColourNb][BoardPieceCapacity];
   uint8 piece_size[ColourNb];

   board_sq_t pawn[ColourNb][BoardPawnCapacity];
   uint8 pawn_size[ColourNb];

   uint8 piece_nb;
   uint8 number[12]; // one counter per coloured piece kind

   uint8 pawn_file[ColourNb][FileNb]; // eight rank bits per file

   uint8 turn;
   uint8 flags;
   uint8 ep_square;
   gc_int ply_nb;
   gc_int sp; // TODO: MOVE ME?

   uint8 cap_sq;

   gc_int opening;
   gc_int endgame;

   uint64 key;
   uint64 pawn_key;
   uint64 material_key;

   uint64 stack[StackSize];
};

// functions

extern bool board_is_ok         (const board_t * board);

extern void board_clear         (board_t * board);
extern void board_copy          (board_t * dst, const board_t * src);

extern void board_init_list     (board_t * board);

extern bool board_is_legal      (const board_t * board);
extern bool board_is_check      (const board_t * board);
extern bool board_is_mate       (const board_t * board);
extern bool board_is_stalemate  (board_t * board);

extern bool board_is_repetition (const board_t * board);

extern gc_int  board_material      (const board_t * board);
extern gc_int  board_opening       (const board_t * board);
extern gc_int  board_endgame       (const board_t * board);

}  // namespace engine

#endif // !defined BOARD_H

// end of board.h

