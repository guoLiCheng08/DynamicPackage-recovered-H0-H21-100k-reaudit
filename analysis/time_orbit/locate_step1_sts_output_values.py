#!/usr/bin/env python3
from pathlib import Path
import struct

root = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit')
sts = (root / 'gold_dyn_main_step1_sts.bin').read_bytes()
out = (root / 'gold_dyn_main_step1_out.bin').read_bytes()
for tracker in range(3):
    print(f'STS output[{tracker}]')
    for element in range(4):
        bits = struct.unpack_from('<Q', out, tracker * 0x20 + element * 8)[0]
        found = []
        needle = struct.pack('<Q', bits)
        start = 0
        while True:
            at = sts.find(needle, start)
            if at < 0:
                break
            found.append(at)
            start = at + 1
        where = ','.join(f'+0x{x:x}' for x in found) if found else 'not-found'
        print(f'  element[{element}] bits=0x{bits:016x} at {where}')
