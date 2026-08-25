#!/usr/bin/env python3
from pathlib import Path
import struct

root = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit')
a = (root / 'gold_core_dynamic_torque_step1.bin').read_bytes()
b = (root / 'gold_core_dynamic_torque_step2.bin').read_bytes()
for offset in range(0, len(a), 8):
    av = struct.unpack_from('<Q', a, offset)[0]
    bv = struct.unpack_from('<Q', b, offset)[0]
    if av != bv:
        ad = struct.unpack_from('<d', a, offset)[0]
        bd = struct.unpack_from('<d', b, offset)[0]
        print(f'+0x{offset:03x}: step1=0x{av:016x} ({ad:.17g}) step2=0x{bv:016x} ({bd:.17g})')
