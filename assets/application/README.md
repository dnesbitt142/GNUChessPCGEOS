# Application artwork inputs

The BMPs preserve the earlier knight and checkerboard artwork from
`gnuchess-pcgeos-icons.zip`. The current desktop knight and white tile
are drawn as pixel art in `tools/generate_appicons.py`.

Run these commands to regenerate and check the compiled GEOS monikers.

```sh
python3 tools/generate_appicons.py
python3 tools/generate_appicons.py --check
```

The generator uses only the Python standard library. It scales the
desktop tile to each display size, including a wider raster for CGA's
non-square pixels. Native scanlines contain a positive opacity mask
followed by pixel data. The generator embeds the bitmaps in GStrings.

`Appl/GNUChess/appicons.goh` contains these selectable native visual monikers:

| Dimensions | Moniker size | Display | Pixel aspect |
| --- | --- | --- | --- |
| 32 × 20 | tiny | color and monochrome | normal |
| 48 × 30 | standard | color and monochrome | normal |
| 64 × 40 | large | color and monochrome | normal |
| 48 × 14 | standard | monochrome CGA | verySquished |
| 15 × 15 | tiny tool | color and monochrome | normal |

The tool monikers use a pixel-fitted black knight defined in the
generator. ISUI uses them for the window and taskbar.
