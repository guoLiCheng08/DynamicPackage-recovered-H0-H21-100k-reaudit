#!/usr/bin/env python3
from pathlib import Path
import struct

expected = Path("analysis/time_orbit/dyn_main_array_gold_out.bin").read_bytes()
actual = Path("build/dyn_main_array_recovered_out.bin").read_bytes()
for offset in range(0, min(len(expected), len(actual)), 8):
    want = struct.unpack_from("<Q", expected, offset)[0]
    got = struct.unpack_from("<Q", actual, offset)[0]
    if want != got:
        print(f"+0x{offset:03x}: expected=0x{want:016x} actual=0x{got:016x}")
