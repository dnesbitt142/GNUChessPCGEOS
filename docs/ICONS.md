# Native chess-piece and application artwork

The icon update embeds the artwork in `gnuchess.geo`. No PNG/BMP decoder, external image file, or additional target library is needed at runtime.

## Board pieces

The six black pieces have been repainted with image generation to give their charcoal bodies stronger highlights, reflected light, and visible three-dimensional shading. The white pieces retain their original design, using the existing 48×48 exports. All twelve production PNGs in `assets/pieces` are now 48×48 with transparent backgrounds.

`pieceicons.goh` contains native GEOS uncompressed masked eight-bit bitmaps. Each bitmap is 2,598 bytes: a six-byte `Bitmap` header and 48 scanlines, each with a six-byte positive opacity mask followed by 48 color-index bytes. Mask bits are set where source alpha is at least 128. Pixel values use the sixteen neutral shades in the default GEOS palette, indices `0x10` through `0x1F` (RGB 0, 17, 34, …, 255). No custom palette is embedded or installed. This avoids the earlier reduction to only four neutral colors, which erased much of the black pieces' shading.

The board keeps its original logical geometry: 28-unit cells and a 24-unit piece footprint at a two-unit inset. `GrDrawBitmap` draws each 48-pixel bitmap through a one-half scale, with the whole board then scaled uniformly to fit the available view. The view has a 264×264 minimum, grows with the application window, and centers the board when the available width and height differ. It has no scrollbars. Mouse coordinates are translated and divided by the same scale used for drawing; clicks on labels or the surrounding margins are ignored. Selection is outlined after the piece is drawn so the highlight remains visible. Empty squares do not draw a bitmap. A low-memory artwork-lock failure falls back to the previous letter glyphs.

The full range of shades is available on 256-color and high-color displays. GEOS can convert the bitmaps for older 16-color and monochrome drivers, but those displays cannot reproduce all of the shading. Enlarging the window exposes more of the detail in the 48-pixel artwork; the minimum-size board still has a 24-pixel piece footprint.

`ChessPieces` is one shared, read-only, movable LMem resource. The renderer locks it before drawing, dereferences the appropriate chunk for each piece, and unlocks it after drawing. It does not keep raw pointers into movable artwork across redraws. The extra artwork does not increase the process's fixed engine data or DGROUP.

## Application icon

`appicons.goh` contains native masked bitmap GStrings. Both the application and primary window moniker lists retain the text title and add these display variants:

| Size | Color | Monochrome |
| --- | --- | --- |
| 32×20, tiny | Yes | Yes |
| 48×30, standard | Yes | Yes |
| 64×40, large | Yes | Yes |
| 48×14, CGA | — | Yes |

The icon data occupies a separate shared, read-only LMem resource. Color versions are converted to the GEOS default 16-color palette; no custom palette is installed. The monochrome companion uses a solid knight silhouette for small-display readability. The permanent geode name, application name, and `GnCh` token identity remain the same.

At startup, an existing text-only `GnCh/0` token from the earlier build is upgraded to the new monikers. Existing graphical/custom icons are preserved. A desktop folder already showing the old icon may need a refresh after the first launch. No token database is deleted.

The startup check deliberately uses `TokenLoadMonikerBlock`, then inspects and frees the copied moniker. The supplied `TokenLockTokenMoniker` C wrapper changes DS without restoring it, which violates the assumption made by this Watcom build. A cold-start test caught the resulting fault in an intermediate build; that call is not used in the delivered code.

Runtime testing also exposed horizontal scrolling in the narrow status text field. It now expands to the available width, has a board-width minimum, and selects the beginning after every update so the side to move remains visible.

The board view no longer accepts keyboard focus: its process content handles mouse selection but has no keyboard-navigation handler. This keeps Tab traversal on the buttons and coordinate entry instead of trapping it in the drawing surface. Mouse events are still delivered normally by the GEOS view.

## Regeneration

The generated `.goh` files are included, so ordinary builds need no image-generation service or imaging package. To regenerate after replacing the corresponding source artwork:

```sh
python3 tools/prepare_black_pieces.py # optional: re-extract black tiles; Pillow
python3 tools/generate_pieceicons.py  # requires Pillow
python3 tools/generate_pieceicons.py --check
python3 tools/generate_appicons.py    # Python standard library only
python3 tools/generate_appicons.py --check
./build.sh
```

Source PNGs are in `assets/pieces`; application BMPs and opacity masks are in `assets/application`. The original piece atlas is retained in `assets/masters`. The repainted black-piece master is `assets/masters/black-shaded.png`, and its generation prompt is recorded in `assets/masters/black-shaded-prompt.txt`.

`prepare_black_pieces.py` extracts the six tiles from the new master, downsamples them to match the corresponding white pieces' heights, and aligns them within 48×48 transparent canvases. It performs layout extraction and downsampling; the painted shading comes from the master. The white production PNGs are the original 48-pixel artwork exports. The encoding scripts then convert the production tiles to native bitmaps deterministically. No image generation or conversion runs during gameplay.

The encoding follows the supplied SDK's `CInclude/graphics.h`, `CInclude/gstring.h`, `Appl/SDK_C/Bitmap/bitmap.goc`, and application-moniker examples, with the system palette defined in the PC/GEOS graphics/video sources. `Library/User/Token/tokenC.asm` and `token.asm` establish the token-call behavior. Use the package's current validation report for what was actually tested.
