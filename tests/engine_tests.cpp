/* SPDX-License-Identifier: GPL-3.0-or-later */
#include "gcbridge.h"
#include "fen.h"
#include "hash.h"
#include "move_do.h"
#include "move_gen.h"
#include <cassert>
#include <cstdio>
#include <cstring>

using namespace engine;

// The engine intentionally retains the arena for the lifetime of the process.
extern "C" int __lsan_is_turned_off()
{
    return 1;
}

static uint64 perft(board_t *board, int depth)
{
    list_t moves;
    undo_t undo;
    uint64 total = 0;

    if (depth == 0) return 1;
    gen_legal_moves(&moves, board);
    for (gc_int i = 0; i < moves.size; ++i) {
        uint64 key = board->key;
        gc_int move = moves.move[i];
        move_do(board, move, &undo);
        assert(board->key == hash_key(board));
        total += perft(board, depth - 1);
        move_undo(board, move, &undo);
        assert(board->key == key && board->key == hash_key(board));
    }
    return total;
}

int main()
{
    board_t board;
    char before[128], after[128], move[6], squares[64];

    GCGetSquares(squares);
    for (char square : squares) assert(square == ' ');
    assert(pool_used() == 0);
    assert(GCInit());
    assert(pool_used() == 34260);
    board_from_fen(&board, StartFen);
    assert(perft(&board, 4) == 197281);
    board_from_fen(&board, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    assert(perft(&board, 3) == 97862);
    board_from_fen(&board, "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    assert(perft(&board, 4) == 43238);
    board_from_fen(&board, "7k/8/8/8/QQQN4/QQBQ4/QQNR4/KRRB4 w - - 0 1");
    assert(board.piece_size[White] == 16 && board.piece[White][16] == SquareNone);
    assert(perft(&board, 2) > 0);

    GCGetFen(before);
    assert(!GCPlay("e2e5") && !GCPlay("e2e4garbage") && !GCPlay("a9a1"));
    GCGetFen(after);
    assert(std::strcmp(before, after) == 0);
    assert(GCPlay("e2e4") && GCEngine(4, move) && GCTurn() == 0);
    assert(GCUndo() && !GCUndo());
    GCGetFen(after);
    assert(std::strcmp(before, after) == 0);

    assert(GCPlay("f2f3") && GCPlay("e7e5") && GCPlay("g2g4") && GCPlay("d8h4"));
    assert(GCStatus() == 2 && !GCEngine(2, move));
    GCTestSetFen("7k/5K2/6Q1/8/8/8/8/8 b - - 0 1");
    assert(GCStatus() == 3);
    GCTestSetFen("7k/8/8/8/8/8/P7/7K w - - 100 1");
    assert(GCStatus() == 5);
    GCTestSetFen("7k/8/8/8/8/8/8/6NK w - - 0 1");
    assert(GCStatus() == 6);

    GCTestSetFen("4k3/P7/8/8/8/8/8/4K3 w - - 0 1");
    assert(GCPlay("a7a8n"));
    GCGetSquares(squares);
    assert(squares[56] == 'N');
    GCTestSetFen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    assert(GCPlay("e1g1"));
    GCGetSquares(squares);
    assert(squares[6] == 'K' && squares[5] == 'R');
    GCTestSetFen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
    assert(GCPlay("e5d6"));
    GCGetSquares(squares);
    assert(squares[43] == 'P' && squares[35] == ' ');

    GCNewGame();
    for (int i = 0; i < 2; ++i) {
        assert(GCPlay("g1f3") && GCPlay("g8f6") && GCPlay("f3g1") && GCPlay("f6g8"));
    }
    assert(GCStatus() == 4);
    GCNewGame();
    for (int i = 0; i < 60 && GCStatus() < 2; ++i) {
        assert(GCEngine(2, move));
    }
    assert(GCInit() && pool_used() == 34260);
    std::printf("Engine smoke test passed (arena used: %ld bytes).\n", (long)pool_used());
}
