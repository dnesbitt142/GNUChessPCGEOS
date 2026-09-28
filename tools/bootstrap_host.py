#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0
"""Build Linux x86-64 GOC and Glue from the supplied PC/GEOS source tree.

The historical on-disk structures are packed and explicitly 32-bit. The
patch keeps these structures independent of the Linux LP64 host ABI.
This does not modify the user's PC/GEOS source or build any target library.
"""
from pathlib import Path
import argparse
import concurrent.futures
import os
import re
import shutil
import subprocess
from config import ROOT, required_dir


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--sanitize', action='store_true', help='instrument host utilities with AddressSanitizer')
    parser.add_argument('--jobs', type=int, default=8)
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    pc = required_dir('ROOT_DIR')
    for executable in ('gcc', 'ar', 'patch'):
        if not shutil.which(executable):
            raise SystemExit(f'Missing host prerequisite: {executable}')
    build = ROOT / 'build/host-tools'
    tools = build / 'Tools'
    (build / 'bin').mkdir(parents=True, exist_ok=True)
    # Only disposable copies in this project's build directory are replaced.
    if tools.exists():
        shutil.rmtree(tools)
    for group in ('include', 'utils', 'compat', 'goc', 'glue'):
        source = pc / 'Tools' / group
        if not source.is_dir():
            raise SystemExit(f'PC/GEOS sources missing: {source}')
        shutil.copytree(source, tools / group)
    # The uploaded Windows ZIP uses CRLF; patches use LF with final newlines.
    for source in tools.rglob('*'):
        if source.is_file() and source.suffix in ('.c', '.h'):
            data = source.read_bytes().replace(b'\r\n', b'\n')
            source.write_bytes(data.rstrip(b'\n') + b'\n')
    with (ROOT / 'tools/pcgeos-host64.patch').open('rb') as patch:
        subprocess.run(['patch', '--batch', '--forward', '-p1'], cwd=build, stdin=patch, check=True)
    # Include system ABI declarations before packing GEOS disk structures.
    headers = ('stdio.h stdlib.h string.h strings.h unistd.h sys/types.h '
               'sys/stat.h sys/time.h sys/resource.h fcntl.h stdint.h time.h '
               'dirent.h stdarg.h').split()
    pre = build / 'pre.h'
    pre.write_text(''.join(f'#include <{h}>\n' for h in headers) + '#pragma pack(push,1)\n')
    sanitize = ['-fsanitize=address'] if args.sanitize else []
    flags = ['gcc', '-std=gnu89', '-w', '-O0', '-g', '-fcommon', '-fno-builtin',
             '-D_LINUX', '-DHAVE_STRERROR', '-DYYDEBUG=1', '-DLEXDEBUG=1',
             '-include', str(pre), '-I'+str(tools/'include'), '-I'+str(tools/'utils')] + sanitize
    for group in ('utils', 'compat', 'goc', 'glue'):
        makefile = pc / 'Installed/Tools' / group / 'Makefile'
        match = re.search(r'linuxOBJS\s*=\s*(.*?)(?=\nlinuxLIBS)', makefile.read_text(), re.S)
        if not match:
            raise SystemExit(f'Cannot find linuxOBJS in {makefile}')
        names = re.findall(r'linux\.md/(\w+)\.o', match.group(1))
        if not names:
            raise SystemExit(f'No objects listed in {makefile}')
        dest = build / group
        dest.mkdir(exist_ok=True)
        def compile_one(name):
            command = flags + ['-I'+str(tools/group)]
            if group == 'goc':
                command += ['-DGOC']
            command += ['-c', str(tools/group/(name+'.c')), '-o', str(dest/(name+'.o'))]
            result = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            (dest/(name+'.log')).write_text(result.stdout)
            return name, result.returncode, result.stdout
        with concurrent.futures.ThreadPoolExecutor(args.jobs) as workers:
            results = list(workers.map(compile_one, names))
        failures = [r for r in results if r[1]]
        if failures:
            for name, _, output in failures:
                print(f'{group}/{name}:\n{output}')
            raise SystemExit(1)
        print(f'{group}: compiled {len(names)} modules', flush=True)
        objects = [str(dest/(name+'.o')) for name in names]
        if group in ('utils', 'compat'):
            command = ['ar', 'rcs', str(build/(group+'.a'))] + objects
        else:
            command = ['gcc'] + sanitize + ['-no-pie', '-o', str(build/'bin'/group)] + objects
            command += [str(build/'utils.a'), str(build/'compat.a'), '-lm']
        result = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        (dest/'link.log').write_text(result.stdout)
        if result.returncode:
            raise SystemExit(result.stdout)
    print(f'Built native host utilities: {build / "bin"}')

if __name__ == '__main__':
    main()
