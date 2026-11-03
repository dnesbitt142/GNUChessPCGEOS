#!/usr/bin/env python3
"""Export the application artwork as native GEOS visual monikers.

Run from any directory: python3 tools/generate_appicons.py [--check]
No Pillow, Icon Editor, or host image converter is needed for rebuilding.

Native format references in the supplied PC/GEOS tree:
  CInclude/{graphics.h,gstring.h}: Bitmap and GSDrawBitmapAtCP formats.
  Library/Kernel/Graphics/graphicsTables.asm: defaultPalette indices 0..15.
  Appl/Install/Instc/art/instal.goh: tiny/standard moniker sizes.
  Appl/CTEdit/Art/icons.goh: large moniker size and interleaved mask rows.

The desktop knight sits on a white tile. Each scanline is a positive mask
(one bit = draw) followed by packed high-nibble-first color pixels, or by
monochrome pixels (one bit = black). Rows are top-down and have no BMP padding.
"""

import argparse
import hashlib
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "Appl/GNUChess/appicons.goh"
# name, dimensions, UI size, aspect, format, moniker style
VARIANTS = (
    ("GCAppIcon48Color", 48, 30, "standard", "normal", "color4", "icon"),
    ("GCAppIcon48Mono", 48, 30, "standard", "normal", "gray1", "icon"),
    ("GCAppIcon32Color", 32, 20, "tiny", "normal", "color4", "icon"),
    ("GCAppIcon32Mono", 32, 20, "tiny", "normal", "gray1", "icon"),
    ("GCAppIcon64Color", 64, 40, "large", "normal", "color4", "icon"),
    ("GCAppIcon64Mono", 64, 40, "large", "normal", "gray1", "icon"),
    ("GCAppIconCGA", 48, 14, "standard", "verySquished", "gray1", "icon"),
    ("GCAppToolColor", 15, 15, "tiny", "normal", "color4", "tool"),
    ("GCAppToolMono", 15, 15, "tiny", "normal", "gray1", "tool"),
)

# White tile with a charcoal border and a black knight.
DESKTOP_KNIGHT = (
    "..ssssssssssssssssssssssssssss..",
    ".swwwwwwwwwwwwwwwwwwwwwwwwwwwws.",
    "swwwwwwwwwwwwwwwwwwwwwwwwwwwwwws",
    "swwwwwwwwwwwwwwwwwwwwwwwwwwwwwws",
    "swwwwwwwwwwwwwwwwwwwwwwwwwwwwwws",
    "swwwwwwwwwwwwwww#wwwwwwwwwwwwwws",
    "swwwwwwwwwwwwwww##wwwwwwwwwwwwws",
    "swwwwwwwwwwwwww######wwwwwwwwwws",
    "swwwwwwwwwwwwww########wwwwwwwws",
    "swwwwwwwwwwwww#######s##wwwwwwws",
    "swwwwwwwwwww##########s##wwwwwws",
    "swwwwwwwwww############s#wwwwwws",
    "swwwwwwwwww###ww#######s##wwwwws",
    "swwwwwwwww####ww########s#wwwwws",
    "swwwwwwww###############s##wwwws",
    "swwwwww#################s##wwwws",
    "swwwww######s###########s##wwwws",
    "swwwww###sss############s##wwwws",
    "swwwww#########w########s##wwwws",
    "swwwwww######ww#########s##wwwws",
    "swwwwwww####www#########s##wwwws",
    "swwwwwwwwwwwww#########s##wwwwws",
    "swwwwwwwwwwww##########s##wwwwws",
    "swwwwwwwwwwww##########s##wwwwws",
    "swwwwwwwwwww###########s##wwwwws",
    "swwwwwwwww#################wwwws",
    "swwwwwwww###################wwws",
    "swwwwww#####################wwws",
    "swwwwww#####################wwws",
    "swwwwwwwwwwwwwwwwwwwwwwwwwwwwwws",
    ".swwwwwwwwwwwwwwwwwwwwwwwwwwwws.",
    "..ssssssssssssssssssssssssssss..",
)


# Pixel-fitted knight for window decorations and the ISUI taskbar.
# Dot is transparent. The black body has charcoal highlights and a white eye.
TOOL_KNIGHT = (
    "...............",
    "......##.......",
    "......#s##.....",
    ".....#ss###....",
    "....##w#s###...",
    "...######s##...",
    "..######s####..",
    "..####.##s###..",
    ".......#s####..",
    "......#s#####..",
    ".....#s######..",
    "....#########..",
    "...###########.",
    "...#ssssss####.",
    "...###########.",
)


def desktop_knight(width, height, aspect):
    tile_height = height - 2
    tile_width = 28 if aspect == "verySquished" else tile_height
    side = len(DESKTOP_KNIGHT) - 1
    xs = [(x * side + (tile_width - 1) // 2) // (tile_width - 1)
          for x in range(tile_width)]
    ys = [(y * side + (tile_height - 1) // 2) // (tile_height - 1)
          for y in range(tile_height)]
    padding = "." * ((width - tile_width) // 2)
    return (["." * width]
            + [padding + "".join(DESKTOP_KNIGHT[y][x] for x in xs) + padding
               for y in ys]
            + ["." * width])


def pack_bits(row):
    packed = bytearray((len(row) + 7) // 8)
    for x, bit in enumerate(row):
        if bit:
            packed[x // 8] |= 0x80 >> (x % 8)
    return packed


def make_bitmap(variant):
    _, width, height, _, aspect, color, style = variant
    rows = TOOL_KNIGHT if style == "tool" else desktop_knight(width, height, aspect)
    mask = [[pixel != "." for pixel in row] for row in rows]
    pixels = [[{"#": 0, "s": 8, "w": 15, ".": 15}[pixel] for pixel in row]
              if color == "color4" else [pixel in "#s" for pixel in row]
              for row in rows]
    kind = 0x11 if color == "color4" else 0x10
    data = bytearray(struct.pack("<HHBB", width, height, 0, kind))
    for y in range(height):
        data.extend(pack_bits(mask[y]))
        if color == "color4":
            data.extend((pixels[y][x] << 4) |
                        (pixels[y][x + 1] if x + 1 < width else 15)
                        for x in range(0, width, 2))
        else:
            data.extend(pack_bits(pixels[y]))
    if len(data) >= 65536:
        raise ValueError(f"{name}: bitmap exceeds a GEOS segment")
    return bytes(data)


def generate():
    lines = [
        "/* Generated by tools/generate_appicons.py; do not edit by hand.",
        " * Native GEOS masked bitmaps, fixed default 16-color palette.",
        " * Source artwork is defined in the generator.",
        " * Regenerate: python3 tools/generate_appicons.py",
        " */", "", "@start AppIconMonikersResource, data;", "",
    ]
    total = 0
    for variant in VARIANTS:
        name, width, height, size, aspect, color, style = variant
        bitmap = make_bitmap(variant)
        total += len(bitmap) + 10  # Approximate moniker and GString headers.
        source = "Pixel-fitted knight" if style == "tool" else "Knight on white tile"
        lines.extend([
            f"/* {source}; bitmap SHA-256 {hashlib.sha256(bitmap).hexdigest()} */",
            f"@visMoniker {name} = {{",
            f"    size = {size};", f"    style = {style};",
            f"    color = {color};", f"    aspectRatio = {aspect};",
            f"    cachedSize = {width}, {height};", "    gstring {",
            f"        GSDrawBitmapAtCP({len(bitmap)}),",
            f"        Bitmap({width},{height},BMC_UNCOMPACTED,(BMT_MASK | "
            f"{'BMF_4BIT' if color == 'color4' else 'BMF_MONO'})),",
        ])
        for start in range(6, len(bitmap), 12):
            lines.append("        " + ", ".join(
                f"0x{value:02x}" for value in bitmap[start:start + 12]) + ",")
        lines.extend(["        GSEndString()", "    }", "}", ""])
    if total >= 60000:
        raise ValueError("Application icon resource is too large")
    # A process-thread token refresh must not lock UI-thread object resources.
    # Keep this source list and all of its monikers in the plain LMem block.
    lines.extend([
        '@visMoniker GCAppTokenText = "GNU Chess";',
        '@visMoniker GCAppTokenList = list { @GCAppTokenText, ' +
        ', '.join('@' + v[0] for v in VARIANTS) + ' };',
        '', '@end AppIconMonikersResource;', '',
    ])
    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true",
                        help="fail if the checked-in generated source differs")
    args = parser.parse_args()
    generated = generate()
    if args.check:
        if not OUTPUT.is_file() or OUTPUT.read_text() != generated:
            raise SystemExit("appicons.goh is stale; run tools/generate_appicons.py")
        print(f"Application icon source is reproducible ({len(VARIANTS)} native masked monikers).")
    else:
        OUTPUT.write_text(generated)
        print(f"Wrote {OUTPUT.relative_to(ROOT)} ({len(VARIANTS)} native masked monikers).")


if __name__ == "__main__":
    main()
