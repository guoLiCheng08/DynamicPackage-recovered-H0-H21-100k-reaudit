import struct
from pathlib import Path
p=Path('input/DynamicPackage.elf').read_bytes()
# ELF virtual addresses in this PIE's .rodata are file offsets for these targets.
for addr in range(0x11a30,0x11ac0,8):
    raw=p[addr:addr+8]
    if len(raw)==8:
        val=struct.unpack('<d',raw)[0]
        print(f'0x{addr:05x}: {val:.17g}  raw={raw.hex()}')
