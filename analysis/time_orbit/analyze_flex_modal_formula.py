#!/usr/bin/env python3
import re
import struct
from pathlib import Path

SOURCE = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sat_flex_real_matrices.log')
SHAPES = {'C_3x10': (3, 10), 'MA_10x10': (10, 10), 'MD_10x10': (10, 10)}


def decode(word: str) -> float:
    return struct.unpack('<d', int(word, 16).to_bytes(8, 'little'))[0]


def load_blocks() -> dict[str, list[float]]:
    blocks = {name: [] for name in SHAPES}
    active = None
    for line in SOURCE.read_text().splitlines():
        tag = line.strip()
        if tag in SHAPES:
            active = tag
            continue
        if active is None or ':' not in line:
            continue
        count = SHAPES[active][0] * SHAPES[active][1]
        if len(blocks[active]) >= count:
            active = None
            continue
        words = re.findall(r'0x([0-9a-f]{16})', line)
        blocks[active].extend(decode(word) for word in words[:count - len(blocks[active])])
    for name, (rows, cols) in SHAPES.items():
        if len(blocks[name]) != rows * cols:
            raise RuntimeError(f'{name}: {len(blocks[name])}')
    return blocks


def matvec(values: list[float], rows: int, cols: int, vector: list[float]) -> list[float]:
    output = []
    for row in range(rows):
        total = 0.0
        for col in range(cols):
            total += values[row * cols + col] * vector[col]
        output.append(total)
    return output


def main() -> None:
    blocks = load_blocks()
    c = blocks['C_3x10']
    ma = blocks['MA_10x10']
    md = blocks['MD_10x10']
    alpha = [0.00034907650515847758, 0.00034884790003935219, 0.0001475350372016103]
    eta = [0.001, -0.002, 0.003, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    eta_dot = [0.011, -0.012, 0.013, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    expected = [-0.001917688455130946, 0.0060766366846213126, -0.010167601888417215,
                -7.5686246222649278e-05, 0.00044417440874539103, -3.4520277567791046e-05,
                -0.000122684806109725, 5.4619059107229619e-07, 5.4436426997051446e-06,
                1.2492556471452672e-06]
    ct_alpha = [sum(c[row * 10 + col] * alpha[row] for row in range(3)) for col in range(10)]
    ma_eta = matvec(ma, 10, 10, eta)
    md_dot = matvec(md, 10, 10, eta_dot)
    print('term C^T alpha:', ', '.join(f'{value:.17g}' for value in ct_alpha))
    print('term MA eta:', ', '.join(f'{value:.17g}' for value in ma_eta))
    print('term MD eta_dot:', ', '.join(f'{value:.17g}' for value in md_dot))
    c_ma_eta = matvec(c, 3, 10, ma_eta)
    c_md_dot = matvec(c, 3, 10, md_dot)
    print('term C MA eta:', ', '.join(f'{value:.17g}' for value in c_ma_eta))
    print('term C MD eta_dot:', ', '.join(f'{value:.17g}' for value in c_md_dot))
    print('term -C MA eta:', ', '.join(f'{-value:.17g}' for value in c_ma_eta))
    print('term +0.1 C MD eta_dot:', ', '.join(f'{0.1 * value:.17g}' for value in c_md_dot))
    for signs in ((1, 1, 1), (1, -1, -1), (-1, 1, 1), (-1, -1, -1)):
        candidate = [signs[0] * ct_alpha[i] + signs[1] * ma_eta[i] + signs[2] * md_dot[i]
                     for i in range(10)]
        max_abs = max(abs(candidate[i] - expected[i]) for i in range(10))
        print(f'signs={signs} max_abs={max_abs:.17g} first3=' +
              ', '.join(f'{value:.17g}' for value in candidate[:3]))


if __name__ == '__main__':
    main()
