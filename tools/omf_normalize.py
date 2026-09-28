#!/usr/bin/env python3
"""Give shared data segments a common paragraph alignment for GEOS Glue.

Watcom C and C++ emit different SEGDEF alignments for the same named data
segments. Glue rejects that mix rather than choosing the stricter alignment.
Strengthening both to 16 bytes is safe: offsets inside each input contribution
are unchanged and Glue handles its placement and relocations. Resource/LMem
segments are deliberately not altered. No code or fixup is changed.
"""
from pathlib import Path
import struct,sys
COMMON={'CONST','CONST2','_DATA','_BSS'}
def index(data,p):
 v=data[p];p+=1
 if v&128:v=((v&127)<<8)|data[p];p+=1
 return v,p

def normalize(path):
 source=Path(path).read_bytes();out=bytearray();names=[''];p=0;changes=[]
 while p<len(source):
  if p+3>len(source):raise ValueError('truncated OMF header')
  typ=source[p];length=struct.unpack_from('<H',source,p+1)[0]
  raw=bytearray(source[p:p+3+length]);payload=raw[3:-1]
  if len(raw)!=length+3 or length<1:raise ValueError('truncated OMF record')
  # Watcom -d0 emits an empty Microsoft debug-extension marker.
  # Glue mistakes it for CodeView32 and consumes PUBDEFs as debug records.
  if typ==0x88 and payload in (bytearray(b'\x80\xa1'),bytearray(b'\x80\xe9')) and b'$$SYMBOLS' not in source:
   p+=3+length;changes.append('empty-debug-marker');continue
  if typ==0x96: # LNAMES
   j=0
   while j<len(payload):
    n=payload[j];j+=1;names.append(bytes(payload[j:j+n]).decode('latin1'));j+=n
  if typ in (0x98,0x99): # SEGDEF / SEGDEF32
   attr=payload[0];j=1
   if not attr>>5:j+=3
   j+=4 if typ==0x99 else 2
   ni,j=index(payload,j)
   if names[ni] in COMMON and attr>>5!=3:
    if attr>>5==0:raise ValueError('absolute common segment unsupported')
    raw[3]=(attr&31)|(3<<5)
    raw[-1]=(-sum(raw[:-1]))&255
    changes.append(names[ni])
  out.extend(raw);p+=3+length
 Path(path).write_bytes(out)
 return changes
if __name__=='__main__':
 for arg in sys.argv[1:]:print(arg,':',','.join(normalize(arg)))
