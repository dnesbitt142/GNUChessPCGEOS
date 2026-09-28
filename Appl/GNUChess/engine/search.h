/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* Native fixed-depth search controller; GNU Chess full search is retained. */
#ifndef SEARCH_H
#define SEARCH_H
#include "board.h"
#include "list.h"
#include "move.h"
namespace engine {
const gc_int DepthMax=16;
const gc_int HeightMax=16; // Bounded native 16-bit stack; includes quiescence.
const gc_int SearchNormal=0, SearchShort=1;
const gc_int SearchUnknown=0, SearchUpper=1, SearchLower=2, SearchExact=3;
struct search_input_t { board_t board[1]; list_t list[1]; gc_int depth_limit; };
struct search_info_t { gc_int check_nb,check_inc; bool stop; };
struct search_root_t {
 list_t list[1]; gc_int depth,move,move_pos,move_nb,last_value;
 bool bad_1,bad_2,change,easy,flag;
};
struct search_best_t { gc_int move,value,flags,depth; mv_t pv[HeightMax]; };
struct search_current_t { board_t board[1]; gc_int max_depth; uint32 node_nb; };
extern search_input_t SearchInput[1];
extern search_info_t SearchInfo[1];
extern search_root_t SearchRoot[1];
extern search_current_t SearchCurrent[1];
extern search_best_t SearchBest[1];
bool depth_is_ok(gc_int depth);
bool height_is_ok(gc_int height);
void search_clear();
void search();
void search_update_best();
void search_update_root();
void search_update_current();
void search_check();
}
#endif
