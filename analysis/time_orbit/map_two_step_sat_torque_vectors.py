#!/usr/bin/env python3
from pathlib import Path
import struct

root = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit')
raw = (root / 'gold_core_dynamic_torque_step2.bin').read_bytes()
base = 0x555555400000 + 0x216760
for offset in range(0, 0x80, 0x10):
    count = struct.unpack_from('<I', raw, offset)[0]
    ptr = struct.unpack_from('<Q', raw, offset + 0x08)[0]
    if base <= ptr < base + len(raw):
        rel = ptr - base
        values = [struct.unpack_from('<d', raw, rel + 8*i)[0] for i in range(3)]
        print(f'DpVector header +0x{offset:03x}: count={count} data=+0x{rel:03x} values={values}')
