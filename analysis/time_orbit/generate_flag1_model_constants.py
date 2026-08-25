from pathlib import Path
import struct

source = Path("analysis/time_orbit/gold_inertia_flag1_step1_model.bin")
target = Path("src/dynamic_sat_inertia_flag1_constants.inc")
values = struct.unpack("<314Q", source.read_bytes())
layout = [
    ("dp_flag1_m3_e8_bits", 9),
    ("dp_flag1_m3_448_bits", 9),
    ("dp_flag1_m3_4a8_bits", 9),
    ("dp_flag1_m3_508_bits", 9),
    ("dp_flag1_m3_568_bits", 9),
    ("dp_flag1_m6_bits", 30),
    ("dp_flag1_coupling_bits", 30),
    ("dp_flag1_m9_bits", 9),
    ("dp_flag1_modal_a_bits", 100),
    ("dp_flag1_modal_d_bits", 100),
]
index = 0
lines = ["/* 由原 ELF Update_sat_inertia_xw(flag=1) 的 RK4 前 Sat backing 截取。 */", ""]
for name, count in layout:
    chunk = values[index:index + count]
    index += count
    lines.append(f"static const uint64_t {name}[{count}] = {{")
    for offset in range(0, count, 3):
        lines.append("    " + ", ".join(f"UINT64_C(0x{item:016x})" for item in chunk[offset:offset + 3]) + ",")
    lines.append("};")
    lines.append("")
if index != len(values):
    raise SystemExit("layout length mismatch")
target.write_text("\n".join(lines) + "\n", encoding="utf-8")
print(target)
