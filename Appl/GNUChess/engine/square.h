/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* square.h

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


// square.h

#ifndef SQUARE_H
#define SQUARE_H

// includes

#include "colour.h"
#include "util.h"

namespace engine {

// constants

const gc_int FileNb = 16;
const gc_int RankNb = 16;

const gc_int SquareNb = FileNb * RankNb;

const gc_int FileInc = +1;
const gc_int RankInc = +16;

const gc_int FileNone = 0;

const gc_int FileA = 0x4;
const gc_int FileB = 0x5;
const gc_int FileC = 0x6;
const gc_int FileD = 0x7;
const gc_int FileE = 0x8;
const gc_int FileF = 0x9;
const gc_int FileG = 0xA;
const gc_int FileH = 0xB;

const gc_int RankNone = 0;

const gc_int Rank1 = 0x4;
const gc_int Rank2 = 0x5;
const gc_int Rank3 = 0x6;
const gc_int Rank4 = 0x7;
const gc_int Rank5 = 0x8;
const gc_int Rank6 = 0x9;
const gc_int Rank7 = 0xA;
const gc_int Rank8 = 0xB;

const gc_int SquareNone = 0;

const gc_int A1=0x44, B1=0x45, C1=0x46, D1=0x47, E1=0x48, F1=0x49, G1=0x4A, H1=0x4B;
const gc_int A2=0x54, B2=0x55, C2=0x56, D2=0x57, E2=0x58, F2=0x59, G2=0x5A, H2=0x5B;
const gc_int A3=0x64, B3=0x65, C3=0x66, D3=0x67, E3=0x68, F3=0x69, G3=0x6A, H3=0x6B;
const gc_int A4=0x74, B4=0x75, C4=0x76, D4=0x77, E4=0x78, F4=0x79, G4=0x7A, H4=0x7B;
const gc_int A5=0x84, B5=0x85, C5=0x86, D5=0x87, E5=0x88, F5=0x89, G5=0x8A, H5=0x8B;
const gc_int A6=0x94, B6=0x95, C6=0x96, D6=0x97, E6=0x98, F6=0x99, G6=0x9A, H6=0x9B;
const gc_int A7=0xA4, B7=0xA5, C7=0xA6, D7=0xA7, E7=0xA8, F7=0xA9, G7=0xAA, H7=0xAB;
const gc_int A8=0xB4, B8=0xB5, C8=0xB6, D8=0xB7, E8=0xB8, F8=0xB9, G8=0xBA, H8=0xBB;

const gc_int Dark  = 0;
const gc_int Light = 1;

// macros

#define SQUARE_IS_OK(square)        ((((square)-0x44)&~0x77)==0)

#define SQUARE_MAKE(file,rank)      (((rank)<<4)|(file))

#define SQUARE_FILE(square)         ((square)&0xF)
#define SQUARE_RANK(square)         ((square)>>4)

#define SQUARE_FROM_64(square)      (SquareFrom64[square])
#define SQUARE_TO_64(square)        (SquareTo64[square])

#define SQUARE_IS_PROMOTE(square)   (SquareIsPromote[square])
#define SQUARE_EP_DUAL(square)      ((square)^16)

#define SQUARE_COLOUR(square)       (((square)^((square)>>4))&1)

#define SQUARE_FILE_MIRROR(square)  ((square)^0x0F)
#define SQUARE_RANK_MIRROR(square)  ((square)^0xF0)

#define FILE_OPP(file)              ((file)^0xF)
#define RANK_OPP(rank)              ((rank)^0xF)

#define PAWN_RANK(square,colour)    (SQUARE_RANK(square)^RankMask[colour])
#define PAWN_PROMOTE(square,colour) (PromoteRank[colour]|((square)&0xF))

// types

typedef gc_int sq_t;

// "constants"

extern const gc_int SquareFrom64[64];
extern const gc_int RankMask[ColourNb];
extern const gc_int PromoteRank[ColourNb];

// variables

extern gc_int SquareTo64[SquareNb];
extern bool SquareIsPromote[SquareNb];

// functions

extern void square_init        ();

extern gc_int  file_from_char     (gc_int c);
extern gc_int  rank_from_char     (gc_int c);

extern gc_int  file_to_char       (gc_int file);
extern gc_int  rank_to_char       (gc_int rank);

extern bool square_to_string   (gc_int square, char string[], gc_int size);
extern gc_int  square_from_string (const char string[]);

}  // namespace engine

#endif // !defined SQUARE_H

// end of square.h

