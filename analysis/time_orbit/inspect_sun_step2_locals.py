#!/usr/bin/env python3
from pathlib import Path
import struct
raw = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sun_step2_locals.bin').read_bytes()
for off in range(0, len(raw), 8):
    bits = struct.unpack_from('<Q', raw, off)[0]
    val = struct.unpack_from('<d', raw, off)[0]
    print(f'+0x{off:02x}: {val:.17g} bits=0x{bits:016x}')
