#!/usr/bin/env python3
"""Package sources, final binary, licenses and current build evidence."""
# SPDX-License-Identifier: GPL-3.0-or-later
from pathlib import Path
import argparse
import hashlib
import json
import zipfile
from config import ROOT


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    report = json.loads((ROOT/'dist/build-report.json').read_text())
    binary = ROOT/'dist/gnuchess.geo'
    actual = hashlib.sha256(binary.read_bytes()).hexdigest()
    if actual != report['sha256']:
        raise SystemExit('Binary and build report differ; rebuild and inspect first.')
    files = [ROOT/name for name in ('README.md','COPYING','AUTHORS.upstream','THANKS.upstream','build.sh','.gitignore')]
    for folder in ('src','tools','tests','docs','assets','LICENSES','vendor','dist'):
        files += [p for p in (ROOT/folder).rglob('*')
                  if p.is_file() and '__pycache__' not in p.parts and p.suffix != '.pyc']
    log_names = ['bootstrap-host.log','build-engine.log','build-geos.log','build-all.log',
                 'goc.log','wcc-ui.log','glue.log','host-tests.log','binary-validation.log',
                 'host-tests-environment.json','icon-tests.log','runtime-tests.log','resize-geometry-tests.log','resize-runtime-tests.log']
    files += [ROOT/'logs'/name for name in log_names if (ROOT/'logs'/name).is_file()]
    files += sorted((ROOT/'logs').glob('cc-*.log'))
    files = sorted(set(files), key=lambda p: str(p.relative_to(ROOT)))
    manifest = ''.join(f'{hashlib.sha256(p.read_bytes()).hexdigest()}  {p.relative_to(ROOT).as_posix()}\n' for p in files)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(args.output, 'w', zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for file in files:
            archive.write(file, 'gnuchess-pcgeos/'+file.relative_to(ROOT).as_posix())
        archive.writestr('gnuchess-pcgeos/SHA256SUMS', manifest)
    with zipfile.ZipFile(args.output) as archive:
        bad = archive.testzip()
        if bad:
            raise SystemExit(f'Archive CRC failure: {bad}')
    print(f'Packaged {len(files)} files plus SHA256SUMS into {args.output} ({args.output.stat().st_size} bytes)')

if __name__ == '__main__':
    main()
