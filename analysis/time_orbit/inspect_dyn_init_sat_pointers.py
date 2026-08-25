#!/usr/bin/env python3
from pathlib import Path
import struct

path = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_init_sat.bin')
raw = path.read_bytes()
base = 0x5555556156E0
ranges = [
    ('Sat', 0x5555556156E0, 0x1078),
    ('DynSat', 0x555555615080, 0x450),
    ('SatTorque', 0x555555616760, 0x168),
    ('WheelGroup', 0x555555617D20, 0xC8),
    ('MTQ_Group', 0x555555617040, 0x110),
    ('Thruster', 0x555555617C60, 0xB0),
]
for offset in range(0, len(raw), 8):
    value = struct.unpack_from('<Q', raw, offset)[0]
    for name, other_base, size in ranges:
        if other_base <= value < other_base + size:
            print(f'Sat+0x{offset:04x}: 0x{value:016x} -> {name}+0x{value-other_base:x}')
            break
