#!/usr/bin/env python3
from pathlib import Path
import struct

image = Path('/home/ubuntu/dynamicpackage_restore/input/DynamicPackage.elf').read_bytes()
addresses = [0xfff8, 0x10030, 0x10038, 0x100d0,
             0x11ad0, 0x11fb0, 0x11fb8, 0x11fc0, 0x11fc8,
             0x11fd0, 0x11fd8, 0x11fe0, 0x11fe8, 0x11ff0, 0x11ff8]
for address in addresses:
    bits = struct.unpack_from('<Q', image, address)[0]
    value = struct.unpack_from('<d', image, address)[0]
    print(f'0x{address:05x}: {value:.17g}  bits=0x{bits:016x}')
