# GNU Chess for PC/GEOS validation

## Berkeley editions

The four geodes were rebuilt with `mkmf`, `pmake depend` and
`pmake -L 4 full` using the supplied SDK sources and Open Watcom.
The German editions were built with `GNUCHESS_LANGUAGE=de`.
Board coordinates, instructions and fallback piece letters use Berkeley
in all four editions. Rank and file labels and the instruction line
follow the board scale, up to Berkeley's 18-point strike.

| Edition | Geode bytes | Initialized resources | Fixed initialized resources |
| --- | ---: | ---: | ---: |
| English NC | 133,922 | 129,247 | 32,028 |
| English EC | 136,282 | 131,640 | 32,028 |
| German NC | 134,530 | 129,853 | 32,828 |
| German EC | 137,114 | 132,470 | 32,828 |

All four pass the geode structure and 8086 instruction audits, including
linked SDK-published routines. They link with `platform geos20`, within
the Ensemble 2.0 protocol limits for GEOS, UI and ANSI C. Runtime testing
covered the supplied Ensemble 6.0 targets. Ensemble 2.0 and physical 286
execution remain unverified.

The linked artwork verifier checked 27,648 piece pixels and masks and
all nine application monikers in each geode. The geometry harness
checked 7,514 points across 13 viewport sizes. Both VM catalogs unpack
and repack byte for byte. The generated German header matches the
German VM string table.

## BaseBox profiles

The low-memory profile uses BaseBox BLD285, the normal 386 core at
3,000 cycles, and XMS, EMS and UMB disabled. DOS `MEM` reports 640 KiB
conventional total and 633 KiB free before GEOS starts.

All four standalone editions opened on their matching NC or EC kernel
at 640 KiB. In the English NC swap run, GeoCalc and GeoWrite were open
and GeoWrite covered the board during two depth-four searches. Switching
back during Thinking restored the board and labels pixel for pixel.
GeoWrite accepted typed input, the engine completed both replies, and
Undo restored the prior position. Maximize and restore also passed.
The swap trace recorded reads of previously written disk blocks.

German EC, with Chess started before GeoWrite, also restored the
complete board during a depth-four search. The viewport matched before
and after GeoWrite covered it. A board click while Thinking left the
position and selection intact. The engine completed the g8–f6 reply,
and the maximized window displayed the correct board.

Starting English NC alongside Calculator, Clock and Perf reproduced
the application's allocation failure. The short retry message fit
at the default window size. Closing Clock and pressing New Game
restored a playable starting position and an engine move completed.
In a German EC run with GeoCalc and GeoWrite open, the full German
message also fit. Neues Spiel allocated the 34,336-byte arena and
restored the starting position while both apps stayed open. Another
simultaneous launch hit the kernel's KR-16 conventional-memory warning.

An English NC installation with the updated German VM opened and
dismissed About during a depth-four search. The uncovered squares
redrew while the German Thinking status was still visible. A click
on a board square during search did not select a square.

The file manager displayed the black knight on its new white tile.
The 15×15 ISUI bitmaps matched the preceding release byte for byte.
Launching with the previous token database replaced the stored desktop
icon with the new bitmap. Captures are in `docs/evidence`.

The host startup regression verifies that reading squares before
engine initialization returns 64 spaces and leaves arena use at zero.

For EC, mount the parent of the Ensemble installation as the DOS C
drive, change into the Ensemble subdirectory, then run `loaderec`.

## Engine regression and original comparison

The engine is shared by all four editions. The host ASan/UBSan smoke
test covers perft, incremental hashes, illegal moves, undo, castling,
en passant, underpromotion, game endings, self-play and depth-four search.
The earlier paired host regression searched 113 positions at depths
1–4 and matched all 3,385 output records from the original engine.
The engine arena remains 34,336 bytes, down from 49,152, with the
48 KiB process stack retained.

In a prior same-profile native NC comparison, pinned app-owned heap
fell from 162,624 bytes to 122,640 bytes. App-owned resident heap after
the first depth-four reply settled went from 171,120 to 144,576 bytes.
Both builds chose b1c3. That one opening took 6.94 seconds on the
original and 6.53 seconds on the compact build, including redraw.

## Geode SHA-256

- English NC `e9bd756b903a17333fd2e45b2b8aa5496c2806b2b4e1b7b1bf0fde55132ae363`
- English EC `e39de23e9368ebd08d6f3af511c3fcc25a433d28cacd155a34adf142373cd1a7`
- German NC `27f86f869cd9bfba6f47423628f0dd8d58c9ef952947afa1a25099ce45f8b0a9`
- German EC `bf05ba928d0031531a878ed9f48a0e3969bd51f63eba45d5d7543d6e960c2dbe`
