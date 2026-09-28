/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* GNU Chess 6.3.0 PC/GEOS public interface. GPL-3.0-or-later.
 * All pointers are far in the native large model. No C++ types cross this ABI.
 */
#ifndef GC_BRIDGE_H
#define GC_BRIDGE_H
#ifdef __WATCOMC__
#define GC_API __far __pascal
#else
#define GC_API
#endif
#ifdef __cplusplus
extern "C" {
#endif
short GC_API GCInit(void); /* 0: not enough memory; safe to retry */
void GC_API GCNewGame(void);
short GC_API GCPlay(const char *coordinate);
short GC_API GCEngine(short depth, char *result /* six bytes */);
short GC_API GCUndo(void); /* one-level undo: human move plus automatic reply */
void GC_API GCGetSquares(char *squares /* 64 bytes, a1 through h8 */);
short GC_API GCTurn(void); /* 0 white, 1 black */
/* 0 playing, 1 check, 2 mate, 3 stalemate, 4 threefold, 5 fifty moves,
 * 6 elementary insufficient material. 0 and 1 permit further play. */
short GC_API GCStatus(void);
unsigned long GC_API GCNodes(void);
void GC_API GCGetFen(char *fen /* at least 128 bytes */);
#ifdef GC_TEST
void GC_API GCTestSetFen(const char *fen); /* trusted, syntactically valid FEN only */
#endif
#ifdef __cplusplus
}
#endif
#endif
