#!/usr/bin/env python3
"""Structural checks for this port's GEOS 2.x executable; not an emulator.

The layout is taken from the supplied Tools/include/{geode,os90File}.h and
Tools/glue/geo.c. It checks resource bounds, relocations, pointer stability,
and (optionally) the supplied target's imported-library protocol versions.
"""
import argparse
import collections
import hashlib
import json
from pathlib import Path
import re
import struct


def require(condition, message):
    if not condition:
        raise ValueError(message)


def inspect(path, map_path, target=None):
    data = path.read_bytes()
    require(len(data) >= 344 and data[:4] == bytes.fromhex('c745c153'), 'Not a GEOS 2.x executable')
    word = lambda p: struct.unpack_from('<H', data, p)[0]
    dword = lambda p: struct.unpack_from('<I', data, p)[0]
    name = data[300:308].decode('ascii').rstrip()
    require(name == 'gnuchess', 'Unexpected permanent geode name')
    count, libraries, exports = word(264), word(266), word(268)
    require(count == word(336) and libraries == word(332) and exports == word(330), 'Inconsistent header counts')
    table = 344 + libraries*14 + exports*4
    require(table+count*10 <= len(data), 'Truncated resource table')
    map_entries = re.findall(r'^([A-Za-z_][A-Za-z_0-9]*)\s+(\d+)\s+(\d+)\s*$', map_path.read_text(), re.M)
    require(len(map_entries) == count, 'Link map/resource count mismatch')
    resources = []
    previous_end = table + count*10
    for i in range(count):
        size = word(table+i*2)
        position = dword(table+count*2+i*4)+256
        reloc_bytes = word(table+count*6+i*2)
        flags = word(table+count*8+i*2)
        reloc_position = position + ((size+15)//16)*16
        end = reloc_position+reloc_bytes
        require(size == int(map_entries[i][1]) and reloc_bytes == int(map_entries[i][2])*4, f'Resource {i}: map mismatch')
        require(reloc_bytes % 4 == 0, f'Resource {i}: invalid relocation size')
        if i:
            require(position >= previous_end and end <= len(data), f'Resource {i}: overlap or out of bounds')
            previous_end = end
        resources.append(dict(id=i, name=map_entries[i][0], initialized_bytes=size,
                              file_offset=position, relocation_offset=reloc_position,
                              relocations=reloc_bytes//4, flags=f'0x{flags:04x}', fixed=bool(flags&0x80)))
    require(previous_end == len(data), 'Unexpected bytes after final resource')
    require(resources[1]['initialized_bytes']+word(270) <= 65536, 'DGROUP exceeds 64 KiB')
    require(word(274) == 1 and word(272) < resources[1]['initialized_bytes'], 'Invalid process class')
    require(word(278) < count and word(276) < resources[word(278)]['initialized_bytes'], 'Invalid application object')
    reloc_types = collections.Counter()
    movable_methods = 0
    for resource in resources[1:]:
        position, size = resource['file_offset'], resource['initialized_bytes']
        for j in range(resource['relocations']):
            info, extra, offset = struct.unpack_from('<BBH', data, resource['relocation_offset']+4*j)
            source, kind = info>>4, info&15
            require(source <= 2 and kind <= 5, 'Unknown relocation encoding')
            require(offset+(4 if kind in (0,4) else 2) <= size, f'{resource["name"]}: relocation outside data')
            reloc_types[f'{source}:{kind}'] += 1
            if source == 1:
                require(extra < libraries, 'Invalid imported-library index')
            if source == 2 and kind in (2,3,4):
                destination = word(position+offset)
                require(destination < count, 'Invalid resource relocation target')
                destination_resource = resources[destination]
                if kind == 4:
                    require(word(position+offset+2) < destination_resource['initialized_bytes'], 'Call outside target code')
                if kind == 2 and not destination_resource['fixed']:
                    # GOC class method tables deliberately contain GEOS virtual
                    # far pointers. The GEOS dispatcher resolves these handles.
                    allowed_method = resource['id'] == 1 and destination_resource['name'] == 'gnuchess_TEXT'
                    require(allowed_method, f'Unsafe raw far pointer to movable {destination_resource["name"]}')
                    movable_methods += 1
    imports = []
    available = {}
    if target:
        require(target.is_dir(), f'Target directory missing: {target}')
        for candidate in target.rglob('*.geo'):
            with candidate.open('rb') as stream:
                header = stream.read(344)
            if len(header) == 344 and header[:4] == bytes.fromhex('c745c153'):
                key = header[300:308].decode('ascii', errors='replace').rstrip()
                available.setdefault(key, []).append((struct.unpack_from('<2H', header, 294), candidate))
    for i in range(libraries):
        p = 344+i*14
        library_name = data[p:p+8].decode('ascii').rstrip()
        major, minor = word(p+10), word(p+12)
        item = dict(name=library_name, required_protocol=f'{major}.{minor}')
        if target:
            compatible = [(protocol, candidate) for protocol, candidate in available.get(library_name, [])
                          if protocol[0] == major and protocol[1] >= minor]
            require(compatible, f'No compatible target library: {library_name} {major}.{minor}')
            protocol, candidate = compatible[0]
            item.update(target_protocol=f'{protocol[0]}.{protocol[1]}', target_file=str(candidate.relative_to(target)))
        imports.append(item)
    return dict(structural_validation='PASS', geos_runtime_tested=False,
                file=path.name, bytes=len(data), sha256=hashlib.sha256(data).hexdigest(),
                permanent_name=name, release='.'.join(str(v) for v in struct.unpack_from('<4H',data,44)),
                notice=data[168:200].split(b'\0')[0].decode('ascii'), resources=count,
                initialized_resource_bytes=sum(r['initialized_bytes'] for r in resources),
                fixed_initialized_bytes=sum(r['initialized_bytes'] for r in resources if r['fixed']),
                dgroup_initialized_bytes=resources[1]['initialized_bytes'],
                dgroup_uninitialized_and_stack_bytes=word(270), process_stack_bytes=49152,
                heap_reservation_bytes=word(260)*16, geos_dispatched_movable_method_pointers=movable_methods,
                relocation_count=sum(reloc_types.values()), relocation_types=dict(reloc_types),
                imports=imports, resource_details=resources)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('geo', type=Path)
    parser.add_argument('--map', required=True, type=Path)
    parser.add_argument('--target', type=Path)
    parser.add_argument('--json', type=Path)
    args = parser.parse_args()
    try:
        result = inspect(args.geo, args.map, args.target)
    except (ValueError, OSError, struct.error) as exc:
        raise SystemExit(f'FAIL: {exc}')
    text = json.dumps(result, indent=2)+'\n'
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(text)
    print(f"PASS: {result['resources']} resources, {result['relocation_count']} relocations, {result['bytes']} bytes")
    print(f"DGROUP: {result['dgroup_initialized_bytes']+result['dgroup_uninitialized_and_stack_bytes']} / 65536 bytes")
    for library in result['imports']:
        print(f"Import {library['name']}: requires {library['required_protocol']}; target {library.get('target_protocol', 'not checked')}")
    print('GEOS execution/UI/loader behaviour: NOT TESTED')
    print('SHA-256: '+result['sha256'])

if __name__ == '__main__':
    main()
