#!/usr/bin/env python3
"""Compile the adapted engine with the supplied 16-bit Open Watcom compiler."""
from pathlib import Path
import subprocess,os,concurrent.futures,sys
from omf_normalize import normalize
from config import ROOT, required_dir, tool
root=ROOT
(root/'logs').mkdir(exist_ok=True)
src=root/'src/engine'; dest=root/'build/engine';dest.mkdir(parents=True,exist_ok=True)
w=required_dir('WATCOM')
tool(w/'binl64/wpp')
sdk=required_dir('GEOS_SDK')
pc=required_dir('ROOT_DIR')
flags=[str(w/'binl64/wpp'),'-zq','-D__GEOS__','-D__WATCOM__','-D_NO_EXT_KEYS','-w3','-fpc','-zu','-of','-s','-ecc','-zp1','-ei','-zdp','-d0','-hc','-zld','-zl','-ml','-zt512','-3','-ox','-i='+str(src),'-i='+str(sdk/'CInclude'),'-i='+str(sdk/'CInclude/Ansi'),'-i='+str(pc/'CInclude'),'-i='+str(w/'h')]
def build(p):
 cmd=flags+['-ntGC'+p.stem.replace('_','').upper(),'-fo='+str(dest/(p.stem+'.obj')),str(p)]
 r=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
 (root/'logs'/('cc-'+p.stem+'.log')).write_text(r.stdout)
 if r.returncode==0:normalize(dest/(p.stem+'.obj'))
 return p.name,r.returncode,r.stdout
res=list(concurrent.futures.ThreadPoolExecutor(8).map(build,sorted(src.glob('*.cpp'))))
for name,code,text in res:
 if code:print('FAILED',name,'\n',text[:6500])
print('Compiled',sum(not code for _,code,_ in res),'of',len(res))
sys.exit(any(code for _,code,_ in res))
