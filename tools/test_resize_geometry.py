#!/usr/bin/env python3
"""Check the actual board resize and hit-test functions with host UBSan.

The functions are extracted from src/geos/gnuchess.goc rather than duplicated.
Only the framework calls are replaced: fixed-point division uses an exact
integer equivalent, and document-bounds/invalidation calls are recorded.
This checks coordinate arithmetic, not GEOS layout or bitmap rendering.

Run: python3 tools/test_resize_geometry.py [--cc gcc]
Requires a host C compiler with UndefinedBehaviorSanitizer support.
The full result is written to logs/resize-geometry-tests.log.
"""
# SPDX-License-Identifier: GPL-3.0-or-later

import argparse
import hashlib
import os
from pathlib import Path
import re
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/geos/gnuchess.goc"
LOG = ROOT / "logs/resize-geometry-tests.log"

HARNESS_PREFIX = r"""
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
typedef int16_t sword;
typedef uint16_t word;
typedef int32_t sdword;
typedef uint32_t WWFixedAsDWord;
typedef uint16_t WindowHandle;
typedef int16_t Boolean;
#define FALSE 0
#define TRUE 1
#define MakeWWFixed(value) ((uint32_t)(value) << 16)
#define WWFixedToInt(value) ((sword)((value) >> 16))

static sword viewPixelWidth=BOARD_W,viewPixelHeight=BOARD_H;
static sword boardOffsetX=0,boardOffsetY=0;
static WWFixedAsDWord boardScale=MakeWWFixed(1);
static unsigned invalidations=0;

/* GEOS unsigned 16.16 division; use a wide host intermediate. */
static WWFixedAsDWord GrUDivWWFixed(WWFixedAsDWord numerator,
                                   WWFixedAsDWord denominator){
    assert(denominator!=0);
    return (uint32_t)(((uint64_t)numerator<<16)/denominator);
}
static void RecordDocBounds(sdword bottom,sdword right,
                            sdword top,sdword left){
    assert(left==0 && top==0);
    assert(right==viewPixelWidth && bottom==viewPixelHeight);
}
static void InvalidateBoard(WindowHandle window){
    assert(window==1);
    invalidations++;
}
"""

HARNESS_MAIN = r"""
int main(void){
    const int sizes[][2]={
        {264,264},{350,264},{264,350},{265,265},{520,264},{264,520},
        {528,528},{790,493},{1024,768},{1366,900},
        {32767,264},{264,32767},{32767,32767}
    };
    /* Sample the center and points two logical pixels inside each edge. */
    const int delta[]={-12,0,12};
    unsigned checks=0;
    unsigned t;
    assert(BOARD_W==264 && BOARD_H==264 && CELL==28);
    for(t=0;t<sizeof(sizes)/sizeof(sizes[0]);t++){
        int width=sizes[t][0],height=sizes[t][1];
        int side=width<height?width:height;
        int square,i,j;
        ResizeBoard((word)width,(word)height,1);
        assert(viewPixelWidth==width && viewPixelHeight==height);
        assert(boardOffsetX==(width-side)/2);
        assert(boardOffsetY==(height-side)/2);
        assert(boardScale==((uint64_t)side*65536)/264);
        assert(invalidations==t+1);
        for(square=0;square<64;square++){
            for(i=0;i<3;i++)for(j=0;j<3;j++){
                int logicalX=LEFT+(square%8)*CELL+CELL/2+delta[i];
                int logicalY=TOP+(7-square/8)*CELL+CELL/2+delta[j];
                /* Forward transform the sampled square position exactly as
                 * drawing does, then test the actual inverse hit function. */
                int pixelX=boardOffsetX+
                    (int)(((uint64_t)logicalX*boardScale)>>16);
                int pixelY=boardOffsetY+
                    (int)(((uint64_t)logicalY*boardScale)>>16);
                assert(BoardSquareAt((sword)pixelX,(sword)pixelY)==square);
                checks++;
            }
        }
        assert(BoardSquareAt(0,0)==-1);checks++;
        assert(BoardSquareAt((sword)(width-1),(sword)(height-1))==-1);
        checks++;
        printf("PASS %dx%d: 64 squares x 9 interior points and outer margins\n",
               width,height);
    }
    assert(checks==7514);
    printf("PASS %u extracted hit-test checks across 13 viewport sizes\n",checks);
    return 0;
}
"""


def extract_function(source, name):
    """GOC functions have a closing brace at column zero in this source."""
    pattern = rf"^static\s+\w+\s+{re.escape(name)}\([^\n]*\)\{{.*?^\}}"
    found = re.findall(pattern, source, re.MULTILINE | re.DOTALL)
    if len(found) != 1:
        raise ValueError(f"Expected exactly one extractable {name} function")
    return found[0]


def make_harness(source):
    defines = []
    for name in ("CELL", "LEFT", "TOP", "BOARD_W", "BOARD_H"):
        matches = re.findall(rf"^#define\s+{name}\s+(\d+)\s*$", source, re.M)
        if len(matches) != 1:
            raise ValueError(f"Expected one numeric {name} definition")
        defines.append(f"#define {name} {matches[0]}")
    resize = extract_function(source, "ResizeBoard")
    resize, count = re.subn(
        r"@call\s+ChessView::MSG_GEN_VIEW_SET_DOC_BOUNDS\s*\((.*?)\);",
        r"RecordDocBounds(\1);", resize, flags=re.DOTALL)
    if count != 1 or "@" in resize:
        raise ValueError("Unexpected GOC call in ResizeBoard; review the harness")
    hit_test = extract_function(source, "BoardSquareAt")
    return "\n".join(defines) + HARNESS_PREFIX + resize + "\n" + hit_test + HARNESS_MAIN


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cc", default=os.environ.get("CC", "gcc"),
                        help="host C compiler command (default: CC or gcc)")
    args = parser.parse_args()
    compiler = shlex.split(args.cc)
    if not compiler:
        raise SystemExit("No host C compiler specified")
    source = SOURCE.read_text()
    harness = make_harness(source)
    lines = [
        "GNU Chess responsive-board coordinate checks",
        "Source: src/geos/gnuchess.goc",
        "Source SHA-256: " + hashlib.sha256(SOURCE.read_bytes()).hexdigest(),
        "Extracted functions: ResizeBoard, BoardSquareAt",
        "Compiler: " + args.cc,
        "Sanitizer: undefined; halt on first error",
        "Scope: host coordinate arithmetic; native GEOS layout/rendering tested separately",
        "",
    ]
    LOG.parent.mkdir(exist_ok=True)
    (ROOT / "build").mkdir(exist_ok=True)
    try:
        with tempfile.TemporaryDirectory(prefix="resize-geometry-", dir=ROOT / "build") as tmp:
            tmp = Path(tmp)
            cfile, binary = tmp / "geometry.c", tmp / "geometry-test"
            cfile.write_text(harness)
            command = compiler + ["-std=c99", "-O2", "-Wall", "-Wextra",
                                  "-fsanitize=undefined", "-fno-sanitize-recover=all",
                                  str(cfile), "-o", str(binary)]
            result = subprocess.run(command, capture_output=True, text=True, timeout=60)
            lines.append(result.stdout + result.stderr)
            if result.returncode:
                raise RuntimeError(f"Compilation failed with exit code {result.returncode}")
            env = dict(os.environ, UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            result = subprocess.run([str(binary)], capture_output=True, text=True,
                                    env=env, timeout=30)
            lines.append(result.stdout + result.stderr)
            if result.returncode:
                raise RuntimeError(f"Geometry checks failed with exit code {result.returncode}")
    except (OSError, RuntimeError, subprocess.TimeoutExpired) as exc:
        lines.append("FAIL: " + str(exc))
        LOG.write_text("\n".join(lines) + "\n")
        raise SystemExit(f"{exc}; see {LOG}")
    LOG.write_text("\n".join(lines) + "\n")
    print(result.stdout, end="")
    print("Log: logs/resize-geometry-tests.log")


if __name__ == "__main__":
    main()
