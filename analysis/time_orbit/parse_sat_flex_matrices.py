#!/usr/bin/env python3
import re
import struct
from pathlib import Path

SOURCE = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sat_flex_real_matrices.log')
OUTPUT = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sat_flex_real_matrices.md')
SHAPES = {
    'M4_3x3': (3, 3),
    'M6_3x10': (3, 10),
    'C_3x10': (3, 10),
    'M9_3x3': (3, 3),
    'MA_10x10': (10, 10),
    'MD_10x10': (10, 10),
}


def decode_word(word: str) -> float:
    return struct.unpack('<d', int(word, 16).to_bytes(8, 'little'))[0]


def c_literal(value: float) -> str:
    return value.hex()


def main() -> None:
    blocks: dict[str, list[float]] = {name: [] for name in SHAPES}
    current: str | None = None
    for line in SOURCE.read_text().splitlines():
        stripped = line.strip()
        if stripped in SHAPES:
            current = stripped
            continue
        if current is None or ':' not in line:
            continue
        expected = SHAPES[current][0] * SHAPES[current][1]
        if len(blocks[current]) >= expected:
            current = None
            continue
        words = re.findall(r'0x([0-9a-f]{16})', line)
        remaining = expected - len(blocks[current])
        blocks[current].extend(decode_word(word) for word in words[:remaining])

    lines = ['# Sat 柔性矩阵真实模型快照', '',
             '由 `sat_flex_real_matrices.gdb` 在原 ELF 主调度首次进入 '
             '`dynamics_flex` 时采集；值以 C99 十六进制浮点字面量记录，保持原始 IEEE-754 位模式。', '']
    for name, (rows, cols) in SHAPES.items():
        expected = rows * cols
        values = blocks[name]
        if len(values) != expected:
            raise RuntimeError(f'{name}: expected {expected} values, got {len(values)}')
        lines.append(f'## {name} ({rows}×{cols})')
        lines.append('')
        lines.append('```c')
        lines.append(f'static const double {name.lower()}[{expected}] = {{')
        for row in range(rows):
            start = row * cols
            lines.append('    ' + ', '.join(c_literal(v) for v in values[start:start + cols]) + ',')
        lines.append('};')
        lines.append('```')
        lines.append('')

    global_shapes = {'H_w_B': 3, 'L_c_B': 3, 'J_c_B': 9, 'J_c_B_inv': 9}
    global_blocks: dict[str, list[float]] = {name: [] for name in global_shapes}
    current = None
    for line in SOURCE.read_text().splitlines():
        if line.strip() == 'H_w_B L_c_B J_c_B J_c_B_inv':
            current = 'H_w_B'
            continue
        if current is None or ':' not in line:
            continue
        words = re.findall(r'0x([0-9a-f]{16})', line)
        global_blocks[current].extend(decode_word(word) for word in words)
        expected = global_shapes[current]
        if len(global_blocks[current]) >= expected:
            if current == 'H_w_B':
                current = 'L_c_B'
            elif current == 'L_c_B':
                current = 'J_c_B'
            elif current == 'J_c_B':
                current = 'J_c_B_inv'
            else:
                current = None
    for name, expected in global_shapes.items():
        values = global_blocks[name]
        if len(values) != expected:
            raise RuntimeError(f'{name}: expected {expected} values, got {len(values)}')
        lines.append(f'## {name} ({expected} values)')
        lines.append('')
        lines.append('```c')
        lines.append(f'static const double {name.lower()}[{expected}] = {{')
        lines.append('    ' + ', '.join(c_literal(value) for value in values) + ',')
        lines.append('};')
        lines.append('```')
        lines.append('')
    OUTPUT.write_text('\n'.join(lines) + '\n')


if __name__ == '__main__':
    main()
