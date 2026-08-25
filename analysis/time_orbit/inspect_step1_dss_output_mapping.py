#!/usr/bin/env python3
from pathlib import Path
import struct

root = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit')
dss = (root / 'gold_dyn_main_step1_dss.bin').read_bytes()
out = (root / 'gold_dyn_main_step1_out.bin').read_bytes()

def f64(blob, offset):
    bits = struct.unpack_from('<Q', blob, offset)[0]
    value = struct.unpack_from('<d', blob, offset)[0]
    return value, bits

for sensor in range(2):
    base = sensor * 0x158
    print(f'DSS[{sensor}]')
    for label, offset in [('angle_x', 0x138), ('ratio_x', 0x140), ('angle_y', 0x148), ('ratio_y', 0x150)]:
        value, bits = f64(dss, base + offset)
        print(f'  {label}: {value:.17g} bits=0x{bits:016x}')
print('UpdateMainOut +0xa0')
for index in range(4):
    value, bits = f64(out, 0xa0 + index * 8)
    print(f'  value[{index}]: {value:.17g} bits=0x{bits:016x}')
