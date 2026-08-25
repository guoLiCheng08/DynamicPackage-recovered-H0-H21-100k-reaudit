#!/usr/bin/env python3
from pathlib import Path
import struct

raw = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_gravity_step2_scale_operands.bin').read_bytes()
numerator = struct.unpack_from('<d', raw, 0)[0]
print(f'original numerator={numerator:.17g} bits=0x{struct.unpack_from("<Q", raw, 0)[0]:016x}')
print(f'original GM=numerator/3={numerator/3.0:.17g} bits=0x{struct.unpack("<Q", struct.pack("<d", numerator/3.0))[0]:016x}')
current = 3.986004418e14
print(f'current GM={current:.17g} bits=0x{struct.unpack("<Q", struct.pack("<d", current))[0]:016x}')
print(f'current numerator={3.0*current:.17g} bits=0x{struct.unpack("<Q", struct.pack("<d", 3.0*current))[0]:016x}')
