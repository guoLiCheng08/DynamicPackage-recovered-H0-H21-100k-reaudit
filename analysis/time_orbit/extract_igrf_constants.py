#!/usr/bin/env python3
from pathlib import Path
import struct

elf = Path('/home/ubuntu/dynamicpackage_restore/input/DynamicPackage.elf').read_bytes()
out = Path('/home/ubuntu/dynamicpackage_restore/src/dynamic_igrf_constants.inc')

# .rodata 的 VMA 与文件偏移均为 0xf7c0，因此这些 VMA 可直接索引样本字节。
def table_bits(vma: int) -> list[list[int]]:
    values = struct.unpack_from('<196Q', elf, vma)
    return [list(values[row * 14:(row + 1) * 14]) for row in range(14)]

def scalar(vma: int) -> tuple[int, float]:
    bits = struct.unpack_from('<Q', elf, vma)[0]
    value = struct.unpack('<d', struct.pack('<Q', bits))[0]
    return bits, value

names = [('dp_igrf_g_dot_bits', 0x10760),
         ('dp_igrf_h_dot_bits', 0x10140),
         ('dp_igrf_g_bits', 0x113a0),
         ('dp_igrf_h_bits', 0x10d80)]
lines = [
    '/* 由 DynamicPackage.elf .rodata 提取；保持每个 IEEE-754 双精度位模式。 */',
    '#ifndef DYNAMIC_IGRF_CONSTANTS_INC',
    '#define DYNAMIC_IGRF_CONSTANTS_INC',
    '',
]
for name, vma in names:
    lines.append(f'static const uint64_t {name}[14][14] = {{')
    for row in table_bits(vma):
        values = ', '.join(f'UINT64_C(0x{value:016x})' for value in row)
        lines.append(f'    {{{values}}},')
    lines.append('};')
    lines.append('')
for name, vma in [('dp_igrf_epoch_bits', 0x119c0),
                  ('dp_igrf_reference_radius_bits', 0x119c8),
                  ('dp_geomagnetic_scale_bits', 0x10120),
                  ('dp_half_pi_bits', 0x10110),
                  ('dp_polar_radius_bits', 0x10118)]:
    bits, value = scalar(vma)
    lines.append(f'/* {name}: {value:.17g} */')
    lines.append(f'#define {name.upper()} UINT64_C(0x{bits:016x})')
lines.extend(['', '#endif /* DYNAMIC_IGRF_CONSTANTS_INC */', ''])
out.write_text('\n'.join(lines))
print(out)
for name, vma in [('epoch', 0x119c0), ('reference_radius', 0x119c8),
                  ('scale', 0x10120), ('half_pi', 0x10110), ('polar_radius', 0x10118)]:
    bits, value = scalar(vma)
    print(f'{name}: {value:.17g} bits=0x{bits:016x}')
