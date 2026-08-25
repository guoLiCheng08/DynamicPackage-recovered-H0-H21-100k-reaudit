#!/usr/bin/env python3
from pathlib import Path
import struct

root = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit')
base = 0x555555400000
objects = [
    ('STS', base + 0x217540, 0x450, 'gold_dyn_main_step1_sts.bin'),
    ('Gyro', base + 0x216A00, 0x640, 'gold_dyn_main_step1_gyro.bin'),
    ('DSS', base + 0x2179A0, 0x2B0, 'gold_dyn_main_step1_dss.bin'),
    ('MagMeter', base + 0x2172C0, 0x270, 'gold_dyn_main_step1_magmeter.bin'),
]
for name, address, size, file_name in objects:
    raw = (root / file_name).read_bytes()
    print(f'[{name}]')
    for offset in range(0, size, 8):
        value = struct.unpack_from('<Q', raw, offset)[0]
        if address <= value < address + size:
            print(f'+0x{offset:03x} -> +0x{value-address:x}')
