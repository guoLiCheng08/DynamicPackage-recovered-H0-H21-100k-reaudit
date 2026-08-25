#!/usr/bin/env python3
"""解析 DynamicPackage 金标文件，输出关键遥测摘要与结构完整性检查。"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

SHM_SIZE = 0xBF0
LOCK_BYTES = 0x38
FLOAT_BASE = 0x38
HEADER = struct.Struct("<8s8I")
FRAME_PREFIX = struct.Struct("<II12f3B1x")
FRAME_SIZE = FRAME_PREFIX.size + SHM_SIZE


def f32_at(shared: bytes, index: int) -> float:
    return struct.unpack_from("<f", shared, FLOAT_BASE + index * 4)[0]


def main(path: Path) -> None:
    data = path.read_bytes()
    if len(data) < HEADER.size:
        raise SystemExit("文件小于金标头部")
    magic, version, shm_size, lock_bytes, count, interval_ms, r0, r1, r2 = HEADER.unpack_from(data)
    if magic != b"DPGOLD01" or version != 1 or shm_size != SHM_SIZE or lock_bytes != LOCK_BYTES:
        raise SystemExit("金标格式或布局不匹配")
    expected = HEADER.size + count * FRAME_SIZE
    if len(data) != expected:
        raise SystemExit(f"文件长度不匹配: expected={expected}, actual={len(data)}")

    frames = []
    for i in range(count):
        offset = HEADER.size + i * FRAME_SIZE
        step, _reserved, *values = FRAME_PREFIX.unpack_from(data, offset)
        command_f32 = list(values[:12])
        command_u8 = list(values[12:15])
        shared_start = offset + FRAME_PREFIX.size
        shared = data[shared_start:shared_start + SHM_SIZE]
        # 锁区应已经被录制器清零，保证比较不受 pthread 内部状态影响。
        lock_zeroed = all(byte == 0 for byte in shared[:LOCK_BYTES])
        frames.append({
            "step": step,
            "command_f32": command_f32,
            "command_u8": command_u8,
            "lock_zeroed": lock_zeroed,
            "sts_q_0": [f32_at(shared, x) for x in range(12, 16)],
            "gyro_0": [f32_at(shared, x) for x in range(24, 27)],
            "magmeter_0": [f32_at(shared, x) for x in range(34, 37)],
            "rw_omega": [f32_at(shared, x) for x in range(40, 44)],
            "gci_position": [f32_at(shared, x) for x in range(50, 53)],
            "gci_velocity": [f32_at(shared, x) for x in range(53, 56)],
        })

    summary = {
        "file": str(path),
        "format": {"magic": magic.decode(), "version": version, "shm_size": shm_size,
                   "lock_bytes_excluded": lock_bytes, "frame_count": count,
                   "interval_ms": interval_ms, "frame_size": FRAME_SIZE},
        "first_frame": frames[0],
        "last_frame": frames[-1],
        "all_lock_regions_zeroed": all(frame["lock_zeroed"] for frame in frames),
        "all_commands_zero": all(
            all(value == 0.0 for value in frame["command_f32"]) and
            frame["command_u8"] == [0, 0, 0]
            for frame in frames
        ),
    }
    print(json.dumps(summary, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit(f"用法: {sys.argv[0]} <golden.bin>")
    main(Path(sys.argv[1]))
