/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* GNU Chess 6.3.0, PC/GEOS fixed-depth controller. GPL-3.0-or-later.
 * Replaces POSIX/UCI timing and longjmp; search_full.cpp remains the engine.
 */
#include "search.h"
#include "search_full.h"
#include "move_gen.h"
#include "trans.h"
#include "sort.h"
#include "pv.h"
#ifdef __WATCOMC__
extern "C" {
#include <timer.h>
}
#endif
namespace engine {
search_info_t SearchInfo[1];
search_root_t SearchRoot[1];
search_current_t SearchCurrent[1];
search_best_t SearchBest[1];
bool depth_is_ok(gc_int d){return d > -128 && d < DepthMax;}
bool height_is_ok(gc_int h){return h >= 0 && h < HeightMax;}
void search_clear(){
 memset(SearchInfo,0,sizeof(*SearchInfo));
 memset(SearchRoot,0,sizeof(*SearchRoot));
 memset(SearchBest,0,sizeof(*SearchBest));
 SearchCurrent->max_depth=0;
 SearchCurrent->node_nb=0;
 SearchInfo->check_inc=1024;
 SearchInfo->check_nb=1024;
}
void search(const board_t *source, gc_int depth_limit){
 gc_int depth;
 // Legal filtering may make temporary moves. Keep them off the source game
 // and generate directly into the root list in the original move order.
 board_copy(SearchCurrent->board,source);
 gen_legal_moves(SearchRoot->list,SearchCurrent->board);
 if (LIST_IS_EMPTY(SearchRoot->list)) return;
 trans_inc_date(Trans);
 sort_init();
 search_full_init(SearchRoot->list,SearchCurrent->board);
 for(depth=1;depth<=depth_limit;depth++){
  board_copy(SearchCurrent->board,source);
  search_full_root(SearchRoot->list,SearchCurrent->board,depth,depth==1?SearchShort:SearchNormal);
 }
}
void search_update_best(){}
void search_update_root(){}
void search_update_current(){}
void search_check(){
#ifdef __WATCOMC__
 TimerSleep(1); // Allow other native threads to run between node batches.
#endif
}
}
