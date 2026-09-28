# Porting notes — updated September 19, 2026

## Engine and ABI

Open Watcom's native `int` is 16 bits, while the engine expects a 32-bit `int`. The adaptation script replaces engine integer declarations with `gc_int` (signed long on Watcom; int32_t in the host test). Packed structures and compile-time integer-size checks are used. Shift constants are explicitly long, including hexadecimal flags. Small stored move/value fields remain 16-bit, and attack delta tables use signed 16-bit entries with bounded values. Full 64-bit position keys are retained; their high halves are copied out without depending on Watcom's unavailable `__U8RS` helper.

`gcbridge.h` exposes only C-compatible scalars and buffers. Its native entry points use far Pascal calling conventions, while engine-internal calls use the large C++ memory model. The interface has no dependency on a C++ run-time constructor system, exceptions, standard-library containers or POSIX threading. The GOC compiler generates ordinary native GEOS objects and methods.

The original engine's Unix front end, opening-book plumbing, protocol driver and desktop search controller are not built. Engine tables are explicitly initialized by `GCInit`. `search_full.cpp` remains the GNU engine's search, driven by an iterative, bounded-depth native controller. There is no floating-point statistics path or nonlocal `setjmp`/`longjmp` cancellation.

## Memory and resources

The process owns one 49,152-byte allocation arena for both boards, options and caches. It is allocated before engine initialization, allowing initial allocation failure to return to the interface for a later retry. Objects and caches are never resized or individually freed; GEOS owns the process lifetime. Host initialization uses 38,340 bytes of this arena; the exact native used-byte count has not been measured at runtime.

The transposition table has 1,024 16-byte entries plus three cluster-spill entries (16,432 bytes). The pawn cache is 512 entries (8,192 bytes); the material cache is 256 entries (4,096 bytes). Board repetition history is bounded to 256 keys. Retained game history is periodically compacted while preserving the window required by the implemented repetition/fifty-move rules.

Watcom `-zt512` splits large arrays and far constants into separate resources. These are fixed because raw far data pointers cannot follow movable GEOS memory. Ordinary inter-resource code calls remain movable through GEOS call relocations. `GCMOVELEGAL` is fixed because `list_filter` calls `pseudo_is_legal` through a raw C++ function pointer; a movable virtual far pointer cannot be used as a normal machine-code call target. GOC class method pointers intentionally remain virtual and are dispatched by GEOS.

Final native layout:

| Component | Bytes |
| --- | ---: |
| Initialized resources, including code and objects | 179,090 |
| Fixed initialized resources (subset of the above) | 60,475 |
| DGROUP initialized portion (subset of the above) | 2,384 |
| Uninitialized DGROUP plus process stack | 50,736 |
| Reserved process stack | 49,152 |
| Total DGROUP allocation | 53,120 |
| Separate engine arena allocation | 49,152 |
| Heap reservation declared in the geode header | 256,000 |

The initialized resources, uninitialized DGROUP and arena total roughly 279 KB before resource padding, kernel handles, UI allocation, library overhead and other system needs. Not all code must be resident at once. This arithmetic is not a measured minimum RAM requirement. The heap-space declaration is a reservation estimate, not proof of sufficient free memory. Use a generously configured GEOS installation and measure actual behaviour there.

Glue warns about fixed memory and several resources larger than its preferred size. They are below the 64 KB segment limit, but the warnings are real and remain in the logs. This port favours a bounded, straightforward first build over aggressive movable-table optimisation.

The hard search height is 16 entries, including quiescence; a static evaluation terminates search at height 15. The requested depth is restricted to 1–4. Extensions and quiescence mean this is not a simple four-ply tree. A checked position can reach the hard cap, so that leaf is evaluated rather than asserting. These limits alter playing behaviour relative to unrestricted GNU Chess.

A Watcom disassembly audit found local frames of 2,810 bytes for `full_search`, 2,246 for `full_no_null`, and 2,264 for quiescence. The 48 KB stack was selected to accommodate the bounded recursion plus outer calls. This is an engineering estimate, not a target-measured stack high-water mark or proof against every stack overflow; target stack testing remains necessary. Automatic compiler stack probes are disabled for the GEOS build as in this toolchain's native calling model.

## Host-tool bootstrap and object normalization

The uploaded SDK contains Windows host utilities. Historical Linux tools assume 32-bit pointers and words in various on-disk structures. `tools/bootstrap_host.py` copies only the required source directories, normalizes their line endings, applies `pcgeos-host64.patch`, and builds native GOC and Glue with GCC. System headers are parsed before enabling packed GEOS disk structures; persistent IDs/dwords/header padding are explicitly 32-bit. An allocation shim uses the host libc in place of the original pointer-size-dependent allocator. Other small patches address compatibility declarations, file handles and scanner stream lifetime. This was verified for this application and a Hello sample, not the whole GEOS tool suite.

`omf_normalize.py` strengthens shared Watcom CONST/CONST2/_DATA/_BSS segment alignment to paragraph alignment so C and C++ contributions agree. It does not change offsets within a contribution or alter instructions/fixups. For objects without actual CodeView symbols, it removes two empty Watcom debug/dependency marker records that the historical linker misinterprets. Every modified OMF record receives a new checksum. GEOS object/LMem resource layouts are left untouched.

C++ engine objects use `-d0 -hc -zld -zl`; the native interface keeps the GOC-required debug/type metadata. The linker imports the supplied SDK's GEOS, UI and ANSIC definitions. No synthetic implementations of missing engine functions were added merely to make linking succeed.

## Validation boundary

The passing host engine test checks move-tree counts and incremental hashes at each move/undo, illegal-input immutability, castling, en passant, underpromotion, mate, stalemate, selected draw rules, undo, depth-four search and self-play. Address/undefined-behaviour sanitizer coverage is limited to that host execution and the stated exclusions.

The binary auditor inspects 59 resources and 969 relocations, verifies the raw-pointer resource policy, and compares protocol versions to the supplied target library headers. It cannot prove that the linker is correct in every detail or that every 16-bit arithmetic/ABI behavior matches the host harness. The artwork verifier independently decodes the actual linked resources and compares pixels, masks, moniker lists, and relocations with the supplied images.

The icon update also received native runtime testing in the supplied SDK target under DOSBox Staging. `docs/TEST-RESULTS.md` records exactly which native interactions passed and which remain untested. The runtime screenshots are actual emulator captures; `dist/embedded-icons-preview.png` is separately labeled as a host reconstruction. See `docs/ICONS.md` for the additional movable artwork resources and the token-wrapper and status-layout fixes found during integration.
