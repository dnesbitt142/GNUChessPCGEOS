# Application artwork inputs

These BMPs are unchanged copies of the application artwork supplied in
`gnuchess-pcgeos-icons.zip`. They contain the previously generated knight and
checkerboard artwork. No font or third-party icon set is used.

The `c` files are color artwork, `a` files their opacity masks, `m` files
monochrome artwork, and `n` files their opacity masks. Mask white means opaque;
mask black means transparent. Monochrome artwork black means a black pixel.

Regenerate the compiled GEOS monikers with:

```sh
python3 tools/generate_appicons.py
python3 tools/generate_appicons.py --check
```

The converter uses only the Python standard library. Color images are exported
to the exact GEOS default 16-color palette using deterministic serpentine
Floyd-Steinberg dithering. Native scanlines contain a positive opacity mask
followed by pixel data. Bitmap data is uncompressed and embedded in GStrings;
the application does not load BMPs at runtime.

`src/geos/appicons.goh` contains these selectable native visual monikers:

| Dimensions | Moniker size | Display | Pixel aspect |
| --- | --- | --- | --- |
| 32 × 20 | tiny | color and monochrome | normal |
| 48 × 30 | standard | color and monochrome | normal |
| 64 × 40 | large | color and monochrome | normal |
| 48 × 14 | standard | monochrome CGA | verySquished |

Together, the seven native bitmap headers and pixel payloads occupy 4,270
bytes. The largest bitmap occupies 1,606 bytes. The resource also contains
small GString and visual-moniker headers.
