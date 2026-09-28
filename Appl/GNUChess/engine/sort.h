/* Adapted for PC/GEOS on 2026-09-18; see docs/PORTING.md. GPL-3.0-or-later. */
#include "porttypes.h"
/* sort.h

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


// sort.h

#ifndef SORT_H
#define SORT_H

// includes

#include "attack.h"
#include "board.h"
#include "list.h"
#include "util.h"

namespace engine {

// types

struct sort_t {
   gc_int depth;
   gc_int height;
   gc_int trans_killer;
   gc_int killer_1;
   gc_int killer_2;
   gc_int gen;
   gc_int test;
   gc_int pos;
   gc_int value;
   board_t * board;
   const attack_t * attack;
   list_t list[1];
   list_t bad[1];
};

// functions

extern void sort_init    ();

extern void sort_init    (sort_t * sort, board_t * board, const attack_t * attack, gc_int depth, gc_int height, gc_int trans_killer);
extern gc_int  sort_next    (sort_t * sort);

extern void sort_init_qs (sort_t * sort, board_t * board, const attack_t * attack, bool check);
extern gc_int  sort_next_qs (sort_t * sort);

extern void good_move    (gc_int move, const board_t * board, gc_int depth, gc_int height);

extern void history_good (gc_int move, const board_t * board);
extern void history_bad  (gc_int move, const board_t * board);

extern void note_moves   (list_t * list, const board_t * board, gc_int height, gc_int trans_killer);

}  // namespace engine

#endif // !defined SORT_H

// end of sort.h

