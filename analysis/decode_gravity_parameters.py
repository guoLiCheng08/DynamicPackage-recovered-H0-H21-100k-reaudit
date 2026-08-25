import struct
from pathlib import Path
p = Path('input/DynamicPackage.elf').read_bytes()
for name, address in [
    ('GM_Earth', 0xffa0), ('R_Earth', 0xffc8),
    ('J2', 0x11a58), ('J3', 0x11a50), ('J4', 0x11a48), ('J5', 0x11a40), ('J6', 0x11a38),
]:
    value = struct.unpack_from('<d', p, address)[0]
    print(f'{name} @ 0x{address:05x} = {value:.17g}')
