#!/usr/bin/env python3
"""Inspect raw original-ELF sensor initialisation snapshots without executing their content."""
from __future__ import annotations

import struct
from pathlib import Path

ROOT = Path('/home/ubuntu/dynamicpackage_restore/analysis/time_orbit')
ENTRIES = {
    'gyro': {
        'base': 0x555555616A00,
        'size': 0x640,
        'items': 2,
        'stride': 0x320,
        'pointers': {'projection_matrix.data': 0x78, 'measure.data': 0x120},
    },
    'magmeter': {
        'base': 0x5555556172C0,
        'size': 0x270,
        'items': 2,
        'stride': 0x138,
        'pointers': {'projection_matrix.data': 0x70, 'measure.data': 0x118},
    },
    'sts': {
        'base': 0x555555617540,
        'size': 0x450,
        'items': 3,
        'stride': 0x170,
        'pointers': {
            'installation_matrix.data': 0x70,
            'error_quat.xyz.data': 0x108,
            'measure_quat.xyz.data': 0x150,
        },
    },
    'dss': {
        'base': 0x5555556179A0,
        'size': 0x2B0,
        'items': 2,
        'stride': 0x158,
        'pointers': {'projection_matrix.data': 0x68, 'measure.data': 0x118},
    },
}

for name, spec in ENTRIES.items():
    raw = (ROOT / f'gold_sensor_init_{name}.bin').read_bytes()
    assert len(raw) == spec['size'], (name, len(raw), spec['size'])
    print(f'[{name}] base=0x{spec["base"]:x} size=0x{spec["size"]:x}')
    for index in range(spec['items']):
        item = index * spec['stride']
        for field, field_offset in spec['pointers'].items():
            pointer = struct.unpack_from('<Q', raw, item + field_offset)[0]
            relative = pointer - spec['base']
            validity = 'inside' if 0 <= relative < spec['size'] else 'external'
            print(f'  item={index} {field}: 0x{pointer:016x}, relative={relative:+#x} ({validity})')
