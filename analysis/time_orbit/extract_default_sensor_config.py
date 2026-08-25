#!/usr/bin/env python3
"""Generate default sensor object byte arrays from controlled original-ELF snapshots."""
from pathlib import Path

ROOT = Path('.')
OUT = ROOT / 'src' / 'dynamic_default_sensor_config.h'
SOURCES = (
    ('sts', ROOT / 'analysis/time_orbit/dyn_main_array_gold_sts.bin'),
    ('gyro', ROOT / 'analysis/time_orbit/dyn_main_array_gold_gyro.bin'),
    ('dss', ROOT / 'analysis/time_orbit/dyn_main_array_gold_dss.bin'),
    ('magmeter', ROOT / 'analysis/time_orbit/dyn_main_array_gold_magmeter.bin'),
    ('rwheel', ROOT / 'analysis/time_orbit/dyn_main_array_gold_rwheel.bin'),
)

lines = [
    '/* Generated from repeatable original ELF dyn_init_array configuration snapshots.',
    ' * Pointer slots are deliberately relocated by the public initializer functions. */',
    '#ifndef DYNAMIC_DEFAULT_SENSOR_CONFIG_H',
    '#define DYNAMIC_DEFAULT_SENSOR_CONFIG_H',
    '#include <stdint.h>',
    '',
]
for name, path in SOURCES:
    blob = path.read_bytes()
    lines.append(f'static const uint8_t dp_default_{name}_bytes[{len(blob)}u] = {{')
    for index in range(0, len(blob), 12):
        row = ', '.join(f'0x{byte:02x}u' for byte in blob[index:index + 12])
        lines.append(f'    {row},')
    lines.append('};')
    lines.append('')
lines.extend(['#endif /* DYNAMIC_DEFAULT_SENSOR_CONFIG_H */', ''])
OUT.write_text('\n'.join(lines), encoding='utf-8')
print(f'wrote {OUT}')
