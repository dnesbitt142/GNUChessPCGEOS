#!/usr/bin/env python3
"""Direct GOC -> Watcom -> Glue build; no pmake or DOS host is required."""
from pathlib import Path
import os, subprocess, sys
from omf_normalize import normalize
from config import ROOT, required_dir, tool
root=ROOT
w=required_dir('WATCOM')
tool(w/'binl64/wcc')
sdk=required_dir('GEOS_SDK')
pc=required_dir('ROOT_DIR')
ht=Path(os.environ.get('GEOS_HOST_TOOLS',str(root/'build/host-tools/bin'))).resolve()
for name in ('goc','glue'):tool(ht/name)
build=root/'build/geos';build.mkdir(parents=True,exist_ok=True)
(root/'logs').mkdir(exist_ok=True)
inc=[sdk/'CInclude',sdk/'CInclude/Ansi',pc/'CInclude',root/'src/engine',root/'src/geos',w/'h']
env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0')
def run(name,cmd):
 print(name,flush=True)
 p=subprocess.run([str(x) for x in cmd],cwd=build,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
 (root/'logs'/f'{name}.log').write_text(p.stdout)
 if p.returncode:print(p.stdout);sys.exit(p.returncode)
run('goc',[ht/'goc','-cw','-D__WATCOM__','-D__GEOS__']+['-I'+str(i) for i in inc]+['-o','gnuchess.c',root/'src/geos/gnuchess.goc'])
run('wcc-ui',[w/'binl64/wcc','-q','-D__GEOS__','-D__WATCOM__','-w3','-fpc','-zu','-of','-s','-ecc','-zp1','-ei','-zdp','-d2','-hc','-ml','-3','-ox']+['-i='+str(i) for i in inc]+['-fo=gnuchess.obj','gnuchess.c'])
normalize(build/'gnuchess.obj')
objs=[root/'build/engine'/(p.stem+'.obj') for p in sorted((root/'src/engine').glob('*.cpp'))]
if not objs or any(not p.is_file() for p in objs):
 raise SystemExit('Engine objects missing. Run tools/compile_engine.py first.')
run('glue',[ht/'glue','-Og',root/'src/geos/gnuchess.gp','-P','1.0','-R','6.3.0.2','-N','GNU Chess contributors; GPLv3+', '-G','2','-L'+str(sdk/'Installed/Include'),'-m','-o','gnuchess.geo','gnuchess.obj']+objs)
print((root/'logs/glue.log').read_text())
