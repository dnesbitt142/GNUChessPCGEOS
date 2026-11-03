# Porting notes

## Engine and ABI

Open Watcom's native `int` is 16 bits, while the engine expects a 32-bit `int`. The adaptation script replaces engine integer declarations with `gc_int` (signed long on Watcom; int32_t in the host test). Packed structures and compile-time integer-size checks are used. Shift constants are explicitly long, including hexadecimal flags. Small stored move/value fields remain 16-bit, and attack delta tables use signed 16-bit entries with bounded values. Full 64-bit position keys are retained; their high halves are copied out without depending on Watcom's unavailable `__U8RS` helper.

`gcbridge.h` exposes only C-compatible scalars and buffers. Its native entry points use far Pascal calling conventions, while engine-internal calls use the large C++ memory model. The interface has no dependency on a C++ run-time constructor system, exceptions, standard-library containers or POSIX threading. The GOC compiler generates ordinary native GEOS objects and methods.

The original engine's Unix front end, opening-book plumbing, protocol driver and desktop search controller are not built. Engine tables are explicitly initialized by `GCInit`. `search_full.cpp` remains the GNU engine's search, driven by an iterative, bounded-depth native controller. There is no floating-point statistics path or nonlocal `setjmp`/`longjmp` cancellation.

## Memory and resources

The process owns one 34,336-byte allocation arena for the game board, undo board, options and caches. The engine searches the game board in place and restores the position before the caller applies the chosen move. It allocates the arena before initializing the engine so a failed allocation can return to the interface for a later retry. The arena stays at a fixed address for the process lifetime.

The compact board, lookup tables and move ordering data come from the Ensemble 1.x port. The new layout retains full 64-bit position keys and the original cache entry counts. Search ordering keeps the score widths needed to reproduce the earlier port's results.

The transposition table has 1,024 16-byte entries plus three cluster-spill entries (16,432 bytes). The pawn cache is 512 entries (8,192 bytes); the material cache is 256 entries (4,096 bytes). Board repetition history is bounded to 256 keys. Retained game history is periodically compacted while preserving the window required by the implemented repetition/fifty-move rules.

Watcom `-zt512` splits large arrays and far constants into separate resources. They are fixed because raw far data pointers cannot follow movable GEOS memory. Ordinary inter-resource code calls remain movable through GEOS call relocations. `GCMOVELEGAL` is fixed because `list_filter` calls `pseudo_is_legal` through a raw C++ far function pointer. GOC class method pointers remain virtual and are dispatched by GEOS.

See [TEST-RESULTS.md](TEST-RESULTS.md) for the linked sizes and BaseBox memory profiles. The heap-space declaration is an estimate used by systems with heap-space limit enforcement. Some resources exceed Glue's recommended size while fitting within the 64 KiB segment limit.

The hard search height is 16 entries, including quiescence; a static evaluation terminates search at height 15. The requested depth is restricted to 1–4. Extensions and quiescence mean this is not a simple four-ply tree. A checked position can reach the hard cap, so that leaf is evaluated rather than asserting. These limits alter playing behaviour relative to unrestricted GNU Chess.

The process retains a 48 KiB stack for recursive search plus GEOS calls and interrupts. The compact sorter reduces the stack needed at each recursive level. The Ensemble 1.x port's highest recursion estimate across its tested configurations was 41,256 bytes before other calls.

The app UI thread owns the visible chessboard and a 64-square snapshot.
The process publishes a position with a synchronous object call, then
searches on its own thread. The UI can repaint or resize using the last
published position during search. Board clicks during search are ignored.
This uses the existing UI and process threads.

Reading squares from an uninitialized engine returns an empty board.
The initial launch and the New Game button explicitly request engine
initialization. New Game can retry after a failed allocation.

## Host-tool bootstrap and object normalization

The uploaded SDK contains Windows host utilities. Historical Linux tools assume 32-bit pointers and words in various on-disk structures. `tools/bootstrap_host.py` copies only the required source directories, normalizes their line endings, applies `pcgeos-host64.patch`, and builds native GOC and Glue with GCC. System headers are parsed before enabling packed GEOS disk structures; persistent IDs/dwords/header padding are explicitly 32-bit. An allocation shim uses the host libc in place of the original pointer-size-dependent allocator. Other small patches address compatibility declarations, file handles and scanner stream lifetime. This was verified for this application and a Hello sample, not the whole GEOS tool suite.

`omf_normalize.py` strengthens shared Watcom CONST/CONST2/_DATA/_BSS segment alignment to paragraph alignment so C and C++ contributions agree. It does not change offsets within a contribution or alter instructions/fixups. For objects without actual CodeView symbols, it removes two empty Watcom debug/dependency marker records that the historical linker misinterprets. Every modified OMF record receives a new checksum. GEOS object/LMem resource layouts are left untouched.

C++ engine objects use `-d0 -hc -zld -zl`; the native interface keeps the GOC-required debug/type metadata. The linker imports the supplied SDK's GEOS, UI and ANSIC definitions. No synthetic implementations of missing engine functions were added merely to make linking succeed.

## Validation boundary

Run `sh tools/test_host.sh` to check move-tree counts, incremental hashes, invalid moves, castling, en passant, promotion, mate, stalemate, draw rules, undo, depth-four search and self-play on the host. The harness runs with AddressSanitizer and UndefinedBehaviorSanitizer.

The binary auditor verifies the raw-pointer resource policy and compares protocol versions to the supplied target library headers. The artwork verifier decodes the linked resources and compares pixels, masks, moniker lists and relocations.

See [ICONS.md](ICONS.md) for native artwork encoding and [TEST-RESULTS.md](TEST-RESULTS.md) for native checks.
