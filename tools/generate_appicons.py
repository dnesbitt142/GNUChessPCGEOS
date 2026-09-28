#!/usr/bin/env python3
"""Export the supplied application artwork as native GEOS visual monikers.

Run from any directory: python3 tools/generate_appicons.py [--check]
No Pillow, Icon Editor, or host image converter is needed for rebuilding.
The source BMPs are unchanged copies of the previously supplied icon assets.

Native format references in the supplied PC/GEOS tree:
  CInclude/{graphics.h,gstring.h}: Bitmap and GSDrawBitmapAtCP formats.
  Library/Kernel/Graphics/graphicsTables.asm: defaultPalette indices 0..15.
  Appl/Install/Instc/art/instal.goh: tiny/standard moniker sizes.
  Appl/CTEdit/Art/icons.goh: large moniker size and interleaved mask rows.

Color conversion is deterministic serpentine Floyd-Steinberg quantization to
the actual GEOS default 16-color palette. This preserves the source artwork's
dark greens using spatial dithering. Each scanline is a positive opaque mask
(one bit = draw) followed by packed high-nibble-first color pixels, or by
monochrome pixels (one bit = black). Rows are top-down and have no BMP padding.
"""

import argparse
import hashlib
from pathlib import Path
import struct

ROOT = Path(__file__).resolve().parents[1]
ASSETS = ROOT / "assets/application"
OUTPUT = ROOT / "src/geos/appicons.goh"
PALETTE = (
    (0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170),
    (170, 0, 0), (170, 0, 170), (170, 85, 0), (170, 170, 170),
    (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255),
    (255, 85, 85), (255, 85, 255), (255, 255, 85), (255, 255, 255),
)
# name, artwork, positive opacity mask, dimensions, UI size, aspect, format
VARIANTS = (
    ("GCAppIcon48Color", "c48x30.bmp", "a48x30.bmp", 48, 30,
     "standard", "normal", "color4"),
    ("GCAppIcon48Mono", "m48x30.bmp", "n48x30.bmp", 48, 30,
     "standard", "normal", "gray1"),
    ("GCAppIcon32Color", "c32x20.bmp", "a32x20.bmp", 32, 20,
     "tiny", "normal", "color4"),
    ("GCAppIcon32Mono", "m32x20.bmp", "n32x20.bmp", 32, 20,
     "tiny", "normal", "gray1"),
    ("GCAppIcon64Color", "c64x40.bmp", "a64x40.bmp", 64, 40,
     "large", "normal", "color4"),
    ("GCAppIcon64Mono", "m64x40.bmp", "n64x40.bmp", 64, 40,
     "large", "normal", "gray1"),
    ("GCAppIconCGA", "m48x14.bmp", "n48x14.bmp", 48, 14,
     "standard", "verySquished", "gray1"),
)


def read_bmp(path):
    """Read the source BI_RGB 1-bit/24-bit Windows BMPs to top-down RGB rows."""
    data = path.read_bytes()
    if data[:2] != b"BM" or len(data) < 54:
        raise ValueError(f"{path}: not a Windows BMP")
    offset, = struct.unpack_from("<I", data, 10)
    dib, width, signed_height, planes, bits, compression = struct.unpack_from(
        "<IiiHHI", data, 14)
    if dib != 40 or width <= 0 or not signed_height or planes != 1:
        raise ValueError(f"{path}: unsupported BMP dimensions/header")
    if bits not in (1, 24) or compression:
        raise ValueError(f"{path}: expected uncompressed 1-bit or 24-bit BMP")
    height = abs(signed_height)
    stride = ((width * bits + 31) // 32) * 4
    if offset + stride * height > len(data):
        raise ValueError(f"{path}: truncated pixels")
    palette = None
    if bits == 1:
        if offset < 62:
            raise ValueError(f"{path}: missing monochrome palette")
        palette = [tuple(data[54 + n * 4:57 + n * 4][::-1]) for n in range(2)]
    rows = []
    for y in range(height):
        sy = height - 1 - y if signed_height > 0 else y
        row = data[offset + sy * stride:offset + (sy + 1) * stride]
        if bits == 24:
            rows.append([tuple(row[x * 3:x * 3 + 3][::-1]) for x in range(width)])
        else:
            rows.append([palette[(row[x // 8] >> (7 - x % 8)) & 1]
                         for x in range(width)])
    return width, height, rows


def opaque_mask(rows):
    """Input mask BMPs explicitly use white for opaque and black for clear."""
    if any(pixel not in ((0, 0, 0), (255, 255, 255))
           for row in rows for pixel in row):
        raise ValueError("Opacity masks must contain only black and white")
    return [[pixel == (255, 255, 255) for pixel in row] for row in rows]


def quantize(rows, mask):
    """Diffuse RGB error within opaque artwork, using exact GEOS indices."""
    height, width = len(rows), len(rows[0])
    work = [[list(map(float, pixel)) for pixel in row] for row in rows]
    result = [[15] * width for _ in range(height)]
    for y in range(height):
        direction = 1 if y % 2 == 0 else -1
        xs = range(width) if direction == 1 else range(width - 1, -1, -1)
        for x in xs:
            if not mask[y][x]:
                continue
            pixel = [max(0.0, min(255.0, channel)) for channel in work[y][x]]
            index = min(range(16), key=lambda n: sum(
                (pixel[c] - PALETTE[n][c]) ** 2 for c in range(3)))
            result[y][x] = index
            error = [pixel[c] - PALETTE[index][c] for c in range(3)]
            for dx, dy, factor in ((direction, 0, 7 / 16),
                                   (-direction, 1, 3 / 16),
                                   (0, 1, 5 / 16), (direction, 1, 1 / 16)):
                nx, ny = x + dx, y + dy
                if 0 <= nx < width and ny < height and mask[ny][nx]:
                    for c in range(3):
                        work[ny][nx][c] += error[c] * factor
    return result


def pack_bits(row):
    packed = bytearray((len(row) + 7) // 8)
    for x, bit in enumerate(row):
        if bit:
            packed[x // 8] |= 0x80 >> (x % 8)
    return packed


def make_bitmap(variant):
    name, artwork, maskfile, width, height, size, aspect, color = variant
    w, h, rows = read_bmp(ASSETS / artwork)
    mw, mh, masks = read_bmp(ASSETS / maskfile)
    if (w, h, mw, mh) != (width, height, width, height):
        raise ValueError(f"{name}: artwork/mask dimensions do not match")
    mask = opaque_mask(masks)
    if color == "color4":
        pixels = quantize(rows, mask)
        kind = 0x11  # BMT_MASK | BMF_4BIT
    else:
        opaque_mask(rows)  # Require exact monochrome input; do not threshold.
        pixels = [[pixel == (0, 0, 0) for pixel in row] for row in rows]
        kind = 0x10  # BMT_MASK | BMF_MONO
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
        " * Source artwork remains in assets/application/.",
        " * Regenerate: python3 tools/generate_appicons.py",
        " */", "", "@start AppIconMonikersResource, data;", "",
    ]
    total = 0
    for variant in VARIANTS:
        name, artwork, maskfile, width, height, size, aspect, color = variant
        bitmap = make_bitmap(variant)
        total += len(bitmap) + 10  # Approximate moniker and GString headers.
        lines.extend([
            f"/* {artwork} + {maskfile}; bitmap SHA-256 {hashlib.sha256(bitmap).hexdigest()} */",
            f"@visMoniker {name} = {{",
            f"    size = {size};", "    style = icon;",
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
        '@visMoniker GCAppTokenText = "GNU Chess 6.3.0";',
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
        print("Application icon source is reproducible (7 native masked monikers).")
    else:
        OUTPUT.write_text(generated)
        print(f"Wrote {OUTPUT.relative_to(ROOT)} (7 native masked monikers).")


if __name__ == "__main__":
    main()
