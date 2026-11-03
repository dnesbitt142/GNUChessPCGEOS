# Native chess-piece and application artwork

The icon update embeds the artwork in `gnuchess.geo`. No PNG/BMP decoder, external image file, or additional target library is needed at runtime.

## Board pieces

The six black pieces have been repainted with image generation to give their charcoal bodies stronger highlights, reflected light, and visible three-dimensional shading. The white pieces retain their original design, using the existing 48×48 exports. All twelve production PNGs in `assets/pieces` are now 48×48 with transparent backgrounds.

`pieceicons.goh` contains native GEOS PackBits-compressed masked eight-bit bitmaps. Each scanline decodes to a six-byte positive opacity mask followed by 48 color-index bytes. Mask bits are set where source alpha is at least 128. Pixel values use the sixteen neutral shades in the default GEOS palette, indices `0x10` through `0x1F` (RGB 0, 17, 34, …, 255). The twelve bitmaps occupy 12,235 bytes, down from 31,176 with the same pixels and masks.

The board keeps its original logical geometry: 28-unit cells and a 24-unit piece footprint at a two-unit inset. `GrDrawBitmap` draws each 48-pixel bitmap through a one-half scale, with the whole board then scaled uniformly to fit the available view. The view has a 264×264 minimum, grows with the application window, and centers the board when the available width and height differ. It has no scrollbars. Mouse coordinates are translated and divided by the same scale used for drawing; clicks on labels or the surrounding margins are ignored. Selection is outlined after the piece is drawn so the highlight remains visible. Empty squares do not draw a bitmap. A low-memory artwork-lock failure falls back to the previous letter glyphs.

The full range of shades is available on 256-color and high-color displays. GEOS can convert the bitmaps for older 16-color and monochrome drivers, but those displays cannot reproduce all of the shading. Enlarging the window exposes more of the detail in the 48-pixel artwork; the minimum-size board still has a 24-pixel piece footprint.

`ChessPieces` is one shared, read-only, movable LMem resource. The renderer locks it only when a piece needs drawing and unlocks it at the end of the pass. Moves, undo, and selection repaint changed squares directly. Exposes and resizes redraw through the normal GEOS update region.

## Application icon

`appicons.goh` contains native masked bitmap GStrings. Both the application and primary window moniker lists retain the text title and add these display variants:

| Size | Color | Monochrome |
| --- | --- | --- |
| 32×20, tiny | Yes | Yes |
| 48×30, standard | Yes | Yes |
| 64×40, large | Yes | Yes |
| 48×14, CGA | — | Yes |
| 15×15, tiny tool | Yes | Yes |

The icon data occupies a separate shared, read-only LMem resource.
The desktop monikers depict a black knight with a white eye on an
outlined white tile. The tool monikers use a transparent background
for window decorations and the ISUI taskbar. The token label is GNU Chess.

At startup, GNU Chess compares the standard desktop icon in its
cached `GnCh/0` token with the compiled icon. It replaces the token
artwork and label when they differ. Reopen an already displayed
desktop folder to see the refreshed icon.

The startup check deliberately uses `TokenLoadMonikerBlock`, then inspects and frees the copied moniker. The supplied `TokenLockTokenMoniker` C wrapper changes DS without restoring it, which violates the assumption made by this Watcom build. A cold-start test caught the resulting fault in an intermediate build; that call is not used in the delivered code.

Runtime testing also exposed horizontal scrolling in the narrow status text field. It now expands to the available width, has a board-width minimum, and selects the beginning after every update so the side to move remains visible.

The board view leaves keyboard focus on the buttons and coordinate
entry. Its visible content handles mouse hit testing and sends selected
squares to the process thread.

Board coordinates, the instruction line and fallback piece letters use
Berkeley in both languages. As the board grows, GEOS selects the
nearest Berkeley bitmap size up to 18 points. The text anchors follow
the same board coordinates at every scale. At the minimum board size,
coordinates use 10 points and the instruction line uses 9 points.

## Regeneration

The generated `.goh` files are included, so ordinary builds need no image-generation service or imaging package. To regenerate after replacing the corresponding source artwork:

```sh
python3 tools/prepare_black_pieces.py # optional: re-extract black tiles; Pillow
python3 tools/generate_pieceicons.py  # requires Pillow
python3 tools/generate_pieceicons.py --check
python3 tools/generate_appicons.py    # Python standard library only
python3 tools/generate_appicons.py --check
cd "$ROOT_DIR/Installed/Appl/GNUChess"
pmake full
```

Source board PNGs are in `assets/pieces`. The application moniker artwork is in `tools/generate_appicons.py`, and `assets/application` holds the earlier BMPs and opacity masks. The original piece atlas is in `assets/masters`. The repainted black-piece master is `assets/masters/black-shaded.png`, and its generation prompt is recorded in `assets/masters/black-shaded-prompt.txt`.

`prepare_black_pieces.py` extracts the six tiles from the new master, downsamples them to match the corresponding white pieces' heights, and aligns them within 48×48 transparent canvases. It performs layout extraction and downsampling; the painted shading comes from the master. The white production PNGs are the original 48-pixel artwork exports. The encoding scripts then convert the production tiles to native bitmaps deterministically. No image generation or conversion runs during gameplay.

The encoding follows the supplied SDK's `CInclude/graphics.h`, `CInclude/gstring.h`, `Appl/SDK_C/Bitmap/bitmap.goc`, and application-moniker examples, with the system palette defined in the PC/GEOS graphics/video sources. `Library/User/Token/tokenC.asm` and `token.asm` establish the token-call behavior. Use the package's current validation report for what was actually tested.
