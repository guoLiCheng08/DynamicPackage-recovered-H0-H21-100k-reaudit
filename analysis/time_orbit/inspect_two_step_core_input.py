#!/usr/bin/env python3
from pathlib import Path
import struct

root = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit')
a = (root / 'gold_core_dynamic_input_step1.bin').read_bytes()
b = (root / 'gold_core_dynamic_input_step2.bin').read_bytes()
for offset in range(0, 0x90, 8):
    av = struct.unpack_from('<d', a, offset)[0]
    bv = struct.unpack_from('<d', b, offset)[0]
    if struct.unpack_from('<Q', a, offset)[0] != struct.unpack_from('<Q', b, offset)[0]:
        print(f'+0x{offset:02x}: step1={av:.17g} step2={bv:.17g}')
print('step2 wheel_group_vector_1 (+0x18):', struct.unpack_from('<3d', b, 0x18))
