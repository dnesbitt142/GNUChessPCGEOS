/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* piece.h

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


// piece.h

#ifndef PIECE_H
#define PIECE_H

// includes

#include "colour.h"
#include "util.h"

namespace engine {

// constants

const gc_int WhitePawnFlag = 1L << 2;
const gc_int BlackPawnFlag = 1L << 3;
const gc_int KnightFlag    = 1L << 4;
const gc_int BishopFlag    = 1L << 5;
const gc_int RookFlag      = 1L << 6;
const gc_int KingFlag      = 1L << 7;

const gc_int PawnFlags  = WhitePawnFlag | BlackPawnFlag;
const gc_int QueenFlags = BishopFlag | RookFlag;

const gc_int PieceNone64 = 0;
const gc_int WhitePawn64 = WhitePawnFlag;
const gc_int BlackPawn64 = BlackPawnFlag;
const gc_int Knight64    = KnightFlag;
const gc_int Bishop64    = BishopFlag;
const gc_int Rook64      = RookFlag;
const gc_int Queen64     = QueenFlags;
const gc_int King64      = KingFlag;

const gc_int PieceNone256   = 0;
const gc_int WhitePawn256   = WhitePawn64 | WhiteFlag;
const gc_int BlackPawn256   = BlackPawn64 | BlackFlag;
const gc_int WhiteKnight256 = Knight64    | WhiteFlag;
const gc_int BlackKnight256 = Knight64    | BlackFlag;
const gc_int WhiteBishop256 = Bishop64    | WhiteFlag;
const gc_int BlackBishop256 = Bishop64    | BlackFlag;
const gc_int WhiteRook256   = Rook64      | WhiteFlag;
const gc_int BlackRook256   = Rook64      | BlackFlag;
const gc_int WhiteQueen256  = Queen64     | WhiteFlag;
const gc_int BlackQueen256  = Queen64     | BlackFlag;
const gc_int WhiteKing256   = King64      | WhiteFlag;
const gc_int BlackKing256   = King64      | BlackFlag;
const gc_int PieceNb        = 256;

const gc_int WhitePawn12   =  0;
const gc_int BlackPawn12   =  1;
const gc_int WhiteKnight12 =  2;
const gc_int BlackKnight12 =  3;
const gc_int WhiteBishop12 =  4;
const gc_int BlackBishop12 =  5;
const gc_int WhiteRook12   =  6;
const gc_int BlackRook12   =  7;
const gc_int WhiteQueen12  =  8;
const gc_int BlackQueen12  =  9;
const gc_int WhiteKing12   = 10;
const gc_int BlackKing12   = 11;

// macros

#define PAWN_MAKE(colour)        (PawnMake[colour])
#define PAWN_OPP(pawn)           ((pawn)^(WhitePawn256^BlackPawn256))

#define PIECE_COLOUR(piece)      (((piece)&3)-1)
#define PIECE_TYPE(piece)        ((piece)&~3)

#define PIECE_IS_PAWN(piece)     (((piece)&PawnFlags)!=0)
#define PIECE_IS_KNIGHT(piece)   (((piece)&KnightFlag)!=0)
#define PIECE_IS_BISHOP(piece)   (((piece)&QueenFlags)==BishopFlag)
#define PIECE_IS_ROOK(piece)     (((piece)&QueenFlags)==RookFlag)
#define PIECE_IS_QUEEN(piece)    (((piece)&QueenFlags)==QueenFlags)
#define PIECE_IS_KING(piece)     (((piece)&KingFlag)!=0)
#define PIECE_IS_SLIDER(piece)   (((piece)&QueenFlags)!=0)

#define PIECE_TO_12(piece)       (PieceTo12[piece])

#define PIECE_ORDER(piece)       (PieceOrder[piece])

#define PAWN_MOVE_INC(colour)    (PawnMoveInc[colour])
#define PIECE_INC(piece)         (PieceInc[piece])

// types

typedef gc_int inc_t;

// "constants"

extern const gc_int PawnMake[ColourNb];
extern const gc_int PieceFrom12[12];

extern const inc_t PawnMoveInc[ColourNb];

extern const inc_t KnightInc[8+1];
extern const inc_t BishopInc[4+1];
extern const inc_t RookInc[4+1];
extern const inc_t QueenInc[8+1];
extern const inc_t KingInc[8+1];

// variables

extern gc_int PieceTo12[PieceNb];
extern gc_int PieceOrder[PieceNb];

extern const inc_t * PieceInc[PieceNb];

// functions

extern void piece_init      ();

extern bool piece_is_ok     (gc_int piece);

extern gc_int  piece_from_12   (gc_int piece_12);

extern gc_int  piece_to_char   (gc_int piece);
extern gc_int  piece_from_char (gc_int c);

}  // namespace engine

#endif // !defined PIECE_H

// end of piece.h

