#!/usr/bin/env python3
"""Generate exact uint64_t model constants from the saved GDB flag=0 snapshot."""
from pathlib import Path
import re

root = Path('/home/ubuntu/dynamicpackage_restore')
log_path = root / 'analysis/time_orbit/update_sat_inertia_flag0_default_full_matrices.log'
out_path = root / 'src/dynamic_sat_inertia_flag0_constants.inc'
labels = [
    'flag0_jc_mem_bits', 'flag0_jc_inv_mem_bits', 'flag0_sat_m3_e8_bits', 'flag0_sat_m3_448_bits',
    'flag0_sat_m3_4a8_bits', 'flag0_sat_m3_508_bits', 'flag0_sat_m3_568_bits',
    'flag0_sat_m6_bits', 'flag0_sat_c_bits', 'flag0_sat_m9_bits',
    'flag0_sat_ma_bits', 'flag0_sat_md_bits',
]
expected_counts = {
    'flag0_jc_mem_bits': 9, 'flag0_jc_inv_mem_bits': 9, 'flag0_sat_m3_e8_bits': 9,
    'flag0_sat_m3_448_bits': 9, 'flag0_sat_m3_4a8_bits': 9,
    'flag0_sat_m3_508_bits': 9, 'flag0_sat_m3_568_bits': 9,
    'flag0_sat_m6_bits': 30, 'flag0_sat_c_bits': 30,
    'flag0_sat_m9_bits': 9, 'flag0_sat_ma_bits': 100,
    'flag0_sat_md_bits': 100,
}
lines = log_path.read_text().splitlines()
blocks = {label: [] for label in labels}
current = None
for line in lines:
    if line in blocks:
        current = line
        continue
    if line.startswith('flag0_'):
        current = None
        continue
    if current is not None and ':\t' in line:
        blocks[current].extend(re.findall(r'0x([0-9a-fA-F]{16})', line))
for label in labels:
    actual = len(blocks[label])
    expected = expected_counts[label]
    if actual != expected:
        raise SystemExit(f'{label}: expected {expected} values, found {actual}')

name_map = {
    'flag0_jc_mem_bits': 'dp_flag0_inertia_bits',
    'flag0_jc_inv_mem_bits': 'dp_flag0_inverse_bits',
    'flag0_sat_m3_e8_bits': 'dp_flag0_m3_e8_bits',
    'flag0_sat_m3_448_bits': 'dp_flag0_m3_448_bits',
    'flag0_sat_m3_4a8_bits': 'dp_flag0_m3_4a8_bits',
    'flag0_sat_m3_508_bits': 'dp_flag0_m3_508_bits',
    'flag0_sat_m3_568_bits': 'dp_flag0_m3_568_bits',
    'flag0_sat_m6_bits': 'dp_flag0_m6_bits',
    'flag0_sat_c_bits': 'dp_flag0_c_bits',
    'flag0_sat_m9_bits': 'dp_flag0_m9_bits',
    'flag0_sat_ma_bits': 'dp_flag0_ma_bits',
    'flag0_sat_md_bits': 'dp_flag0_md_bits',
}
result = [
    '/* Auto-generated from update_sat_inertia_flag0_default_full_matrices.log. */',
    '#ifndef DYNAMIC_SAT_INERTIA_FLAG0_CONSTANTS_INC',
    '#define DYNAMIC_SAT_INERTIA_FLAG0_CONSTANTS_INC',
    '',
]
for label in labels:
    values = blocks[label]
    result.append(f'static const uint64_t {name_map[label]}[{len(values)}] = {{')
    for index in range(0, len(values), 4):
        row = ', '.join(f'UINT64_C(0x{value.lower()})' for value in values[index:index + 4])
        result.append(f'    {row},')
    result.append('};')
    result.append('')
result.append('#endif /* DYNAMIC_SAT_INERTIA_FLAG0_CONSTANTS_INC */')
out_path.write_text('\n'.join(result) + '\n')
print(f'wrote {out_path}')
for label in labels:
    print(f'{label}: {len(blocks[label])}')
