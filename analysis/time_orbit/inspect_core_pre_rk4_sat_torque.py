#!/usr/bin/env python3
from pathlib import Path
import struct

raw = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_core_pre_rk4_sat_torque.bin').read_bytes()
for offset in range(0, len(raw), 8):
    bits = struct.unpack_from('<Q', raw, offset)[0]
    value = struct.unpack_from('<d', raw, offset)[0]
    if bits != 0:
        print(f'SatTorque+0x{offset:03x}: {value:.17g} bits=0x{bits:016x}')
