#!/usr/bin/env python3
from pathlib import Path
import struct

raw = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_dynamic_sat_step2.bin').read_bytes()
base = 0x555555400000 + 0x2156e0
for label, offset in [('inertia_matrix', 0x08), ('position_vector', 0x118), ('attitude_matrix', 0x1a8)]:
    print(label, f'header +0x{offset:x}')
    print('  ints', struct.unpack_from('<4I', raw, offset))
    ptr = struct.unpack_from('<Q', raw, offset+8 if label == 'position_vector' else offset+0x10)[0]
    rel = ptr-base
    print(f'  ptr=0x{ptr:x} rel=0x{rel:x}')
    if 0 <= rel < len(raw):
        print('  values', [struct.unpack_from('<d', raw, rel+8*i)[0] for i in range(9)])
