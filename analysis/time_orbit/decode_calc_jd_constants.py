#!/usr/bin/env python3
"""只读解码 Calc_JD 使用的 .rodata 双精度常量。"""
import struct
from pathlib import Path

binary = Path(__file__).resolve().parents[2] / "input" / "DynamicPackage.elf"
addresses = [
    0xFFE8, 0xFFF0, 0xFFF8, 0x10000, 0x10008, 0x10010, 0x10018, 0x10020,
    0x10028, 0x10030, 0x10038, 0x10040, 0x10048, 0x10050, 0x10058,
    0x10060, 0x10068, 0x10070, 0x10078, 0x10080, 0x10088, 0x10090,
    0x10098, 0x100A0, 0x100A8, 0x100B0, 0x100B8, 0x100C0, 0x10110,
    0x11AC0, 0x11AC8, 0x11AD0, 0x11AD8,
]
with binary.open("rb") as handle:
    for address in addresses:
        handle.seek(address)
        raw = handle.read(8)
        print(f"0x{address:05x} {raw.hex()} {struct.unpack('<d', raw)[0]:.17g}")
