#!/usr/bin/env python3
"""Extract production tiles from the generated black-piece sprite atlas.

This performs layout extraction and downsampling only. The painted shading is
in assets/masters/black-shaded.png; white artwork supplies matching heights.
Requires Pillow. Ordinary builds use the checked-in 48px PNGs and GOC header.
"""
# SPDX-License-Identifier: GPL-3.0-or-later
from pathlib import Path
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
PIECES = ('king', 'queen', 'rook', 'bishop', 'knight', 'pawn')


def main():
    atlas = Image.open(ROOT / 'assets/masters/black-shaded.png').convert('RGBA')
    width, height = atlas.size
    for index, name in enumerate(PIECES):
        column, row = index % 3, index // 3
        tile = atlas.crop((column * width // 3, row * height // 2,
                           (column + 1) * width // 3, (row + 1) * height // 2))
        # Ignore imperceptible generated alpha noise when finding the sprite.
        bounds = tile.getchannel('A').point(lambda a: 255 if a >= 16 else 0).getbbox()
        if bounds is None:
            raise ValueError(f'No opaque artwork for {name}')
        sprite = tile.crop(bounds)
        white = Image.open(ROOT / 'assets/pieces' / f'white-{name}.png').convert('RGBA')
        if white.size != (48, 48):
            raise ValueError('White source tiles must be 48x48')
        white_bounds = white.getbbox()
        target_height = white_bounds[3] - white_bounds[1]
        target_width = round(sprite.width * target_height / sprite.height)
        if target_width > 44:
            raise ValueError(f'{name} is too wide for its production tile')
        sprite = sprite.resize((target_width, target_height), Image.Resampling.LANCZOS)
        output = Image.new('RGBA', (48, 48))
        output.alpha_composite(sprite, ((48 - target_width) // 2, 46 - target_height))
        path = ROOT / 'assets/pieces' / f'black-{name}.png'
        output.save(path)
        print(f'{path.relative_to(ROOT)}: {target_width}x{target_height} artwork in 48x48 tile')


if __name__ == '__main__':
    main()
