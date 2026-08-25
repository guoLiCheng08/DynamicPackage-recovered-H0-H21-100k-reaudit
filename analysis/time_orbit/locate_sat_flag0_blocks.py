#!/usr/bin/env python3
from pathlib import Path
import re
import struct

root = Path('/home/ubuntu/dynamicpackage_restore')
text = (root / 'src/dynamic_sat_inertia_flag0_constants.inc').read_text()
raw = (root / 'analysis/time_orbit/gold_core_pre_rk4_sat.bin').read_bytes()
blocks = re.findall(r'static const uint64_t (dp_flag0_[a-z0-9_]+)_bits\[[0-9]+\] = \{(.*?)\};', text, re.S)
for name, body in blocks:
    values = [int(x, 16) for x in re.findall(r'UINT64_C\(0x([0-9a-fA-F]+)\)', body)]
    if not values:
        continue
    needle = b''.join(struct.pack('<Q', x) for x in values)
    at = raw.find(needle)
    print(f'{name}: count={len(values)} offset=' + (f'0x{at:x}' if at >= 0 else 'not-found'))
