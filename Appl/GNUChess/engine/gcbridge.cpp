/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* Native driver for the GNU Chess 6.3.0 engine. GPL-3.0-or-later. */
#include "gcbridge.h"
#include "attack.h"
#include "hash.h"
#include "fen.h"
#include "move_do.h"
#include "move_gen.h"
#include "option.h"
#include "pawn.h"
#include "piece.h"
#include "random.h"
#include "square.h"
#include "trans.h"
#include "value.h"
#include "vector.h"
#include "material.h"
#include "pst.h"
#include "eval.h"
#include "search.h"
using namespace engine;
static board_t *Game=0, *Previous=0;
static bool UndoValid=false, HumanPending=false;

static void trim_history(board_t *b){
 // Beyond 100 reversible plies the game is adjudicated a fifty-move draw.
 // Preserve all history needed for threefold and enough space for search.
 if(b->sp >= 128){
  gc_int keep=b->ply_nb;
  if(keep>100)keep=100;
  if(keep>b->sp)keep=b->sp;
  memmove(b->stack,b->stack+b->sp-keep,(unsigned short)(keep*sizeof(uint64)));
  b->sp=keep;
 }
}
static void save_undo(){board_copy(Previous,Game);UndoValid=true;}
static void apply(gc_int move){undo_t undo;trim_history(Game);move_do(Game,move,&undo);}

short GC_API GCInit(void){
 if(Game)return 1;
 if(!pool_init())return 0;
 option_init();
 square_init();piece_init();pawn_init_bit();value_init();vector_init();
 attack_init();move_do_init();random_init();hash_init();
 trans_init(Trans);trans_alloc(Trans);
 pawn_init();pawn_alloc();material_init();material_alloc();pst_init();eval_init();
 Game=(board_t*)my_malloc(sizeof(board_t));
 Previous=(board_t*)my_malloc(sizeof(board_t));
 GCNewGame();return 1;
}
void GC_API GCNewGame(void){
 if(!Game){GCInit();return;}
 board_from_fen(Game,StartFen);
 UndoValid=false;HumanPending=false;trans_clear(Trans);pawn_clear();material_clear();
 search_clear();
}
short GC_API GCPlay(const char *s){
 gc_int m,i;list_t legal;
 if(!Game||!s||GCStatus()>1)return 0;
 i=(gc_int)strlen(s);
 if(i!=4 && i!=5)return 0;
 if(s[0]<'a'||s[0]>'h'||s[2]<'a'||s[2]>'h'||s[1]<'1'||s[1]>'8'||s[3]<'1'||s[3]>'8')return 0;
 if(i==5 && s[4]!='q' && s[4]!='r' && s[4]!='b' && s[4]!='n')return 0;
 m=move_from_string(s,Game);
 if(m==MoveNone)return 0;
 gen_legal_moves(&legal,Game);
 for(i=0;i<legal.size;i++)if(legal.move[i]==m){
  save_undo();apply(m);HumanPending=true;return 1;
 }
 return 0;
}
short GC_API GCEngine(short depth,char *result){
 gc_int move;
 if(result)result[0]=0;
 if(!Game||GCStatus()>1)return 0;
 if(depth<1)depth=1;if(depth>4)depth=4;
 trim_history(Game);
 search_clear();board_copy(SearchInput->board,Game);
 SearchInput->depth_limit=depth;search();
 move=SearchBest->move;
 if(move==MoveNone)return 0;
 if(result)move_to_string(move,result,6);
 if(!HumanPending)save_undo();
 apply(move);HumanPending=false;return 1;
}
short GC_API GCUndo(void){
 if(!UndoValid)return 0;
 board_copy(Game,Previous);UndoValid=false;HumanPending=false;
 trans_clear(Trans);return 1;
}
void GC_API GCGetSquares(char *s){
 gc_int i,p;
 if(!Game && !GCInit()){memset(s,' ',64);return;}
 for(i=0;i<64;i++){p=Game->square[SQUARE_FROM_64(i)];s[i]=p==Empty?' ':(char)piece_to_char(p);}
}
short GC_API GCTurn(void){return Game?(short)Game->turn:0;}
short GC_API GCStatus(void){
 list_t legal;gc_int i,n=0,p;
 if(!Game)return 0;
 gen_legal_moves(&legal,Game);
 if(legal.size==0)return board_is_check(Game)?2:3;
 if(Game->ply_nb>=100)return 5;
 for(i=Game->sp-2;i>=0 && i>=Game->sp-Game->ply_nb;i-=2){
  if(Game->stack[i]==Game->key && ++n>=2)return 4;
 }
 if(Game->pawn_size[0]+Game->pawn_size[1]==0){
  if(Game->piece_nb==2)return 6;
  if(Game->piece_nb==3){
   p=Game->piece_size[0]==2?Game->square[Game->piece[0][1]]:Game->square[Game->piece[1][1]];
   if(PIECE_IS_BISHOP(p)||PIECE_IS_KNIGHT(p))return 6;
  }
 }
 return board_is_check(Game)?1:0;
}
unsigned long GC_API GCNodes(void){return (unsigned long)SearchCurrent->node_nb;}
void GC_API GCGetFen(char *fen){board_to_fen(Game,fen,128);}
#ifdef GC_TEST
void GC_API GCTestSetFen(const char *fen){board_from_fen(Game,fen);UndoValid=false;HumanPending=false;trans_clear(Trans);}
#endif
