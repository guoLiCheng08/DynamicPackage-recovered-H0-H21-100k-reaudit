import struct
with open('input/DynamicPackage.elf', 'rb') as f:
    # ELF virtual address .rodata 0xf7c0 对应文件 offset 0xf7c0
    f.seek(0x11ab8)
    raw = f.read(8)
print(raw.hex(), format(struct.unpack('<d', raw)[0], '.17g'))
