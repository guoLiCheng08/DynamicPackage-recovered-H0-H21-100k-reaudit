#!/usr/bin/env python3
"""Extract gh_change's immutable ELF coefficient matrices without modifying the sample."""
from pathlib import Path
import struct

ELF = Path("input/DynamicPackage.elf")
OUT = Path("src/dynamic_gh_change_coeffs.h")
COUNT = 14 * 14
SOURCES = (
    ("h_dot", 0x10140),
    ("g_dot", 0x10760),
    ("h", 0x10D80),
    ("g", 0x113A0),
)
BASE_YEAR_ADDRESS = 0x119C0

blob = ELF.read_bytes()
if len(blob) <= BASE_YEAR_ADDRESS + 8:
    raise SystemExit("ELF unexpectedly too short for gh_change coefficient region")

lines = [
    "/* Generated from the read-only DynamicPackage.elf coefficient region by",
    " * analysis/time_orbit/extract_gh_change_coeffs.py.  Values are IEEE-754",
    " * bit patterns, preserving exact original constants. */",
    "#ifndef DYNAMIC_GH_CHANGE_COEFFS_H",
    "#define DYNAMIC_GH_CHANGE_COEFFS_H",
    "#include <stdint.h>",
    "",
]
for name, address in SOURCES:
    raw = blob[address:address + COUNT * 8]
    if len(raw) != COUNT * 8:
        raise SystemExit(f"truncated {name} matrix")
    words = struct.unpack(f"<{COUNT}Q", raw)
    lines.append(f"static const uint64_t dp_gh_change_{name}_bits[{COUNT}] = {{")
    for index in range(0, COUNT, 4):
        chunk = ", ".join(f"UINT64_C(0x{word:016x})" for word in words[index:index + 4])
        lines.append(f"    {chunk},")
    lines.append("};")
    lines.append("")
base = struct.unpack("<Q", blob[BASE_YEAR_ADDRESS:BASE_YEAR_ADDRESS + 8])[0]
lines.append(f"static const uint64_t dp_gh_change_base_year_bits = UINT64_C(0x{base:016x});")
lines.append("")
lines.append("#endif /* DYNAMIC_GH_CHANGE_COEFFS_H */")
lines.append("")
OUT.write_text("\n".join(lines), encoding="utf-8")
print(f"wrote {OUT} from {ELF}; base-year bits=0x{base:016x}")
