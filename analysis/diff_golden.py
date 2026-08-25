#!/usr/bin/env python3
"""逐帧逐字节比较两个 DPGOLD01 金标文件，输出首差和字节一致率。"""
from __future__ import annotations

import json
import struct
import sys
from pathlib import Path

SHM_SIZE = 0xBF0
LOCK_BYTES = 0x38
HEADER = struct.Struct("<8s8I")
FRAME_PREFIX = struct.Struct("<II12f3B1x")
FRAME_SIZE = FRAME_PREFIX.size + SHM_SIZE


def load(path: Path) -> tuple[tuple, bytes]:
    data = path.read_bytes()
    if len(data) < HEADER.size:
        raise ValueError(f"{path}: 缺少头部")
    head = HEADER.unpack_from(data)
    magic, version, shm_size, lock_bytes, count, _interval, *_rest = head
    if magic != b"DPGOLD01" or version != 1 or shm_size != SHM_SIZE or lock_bytes != LOCK_BYTES:
        raise ValueError(f"{path}: 格式不支持")
    if len(data) != HEADER.size + count * FRAME_SIZE:
        raise ValueError(f"{path}: 长度不匹配")
    return head, data


def main(left_path: Path, right_path: Path) -> None:
    left_header, left = load(left_path)
    right_header, right = load(right_path)
    frame_count = min(left_header[4], right_header[4])
    comparable = frame_count * (SHM_SIZE - LOCK_BYTES)
    equal = 0
    first = None

    for frame_index in range(frame_count):
        base = HEADER.size + frame_index * FRAME_SIZE + FRAME_PREFIX.size
        for offset in range(LOCK_BYTES, SHM_SIZE):
            lv = left[base + offset]
            rv = right[base + offset]
            if lv == rv:
                equal += 1
            elif first is None:
                first = {
                    "frame_index": frame_index,
                    "shared_memory_offset_hex": f"0x{offset:03x}",
                    "left_byte_hex": f"0x{lv:02x}",
                    "right_byte_hex": f"0x{rv:02x}",
                }

    result = {
        "left": str(left_path),
        "right": str(right_path),
        "frame_count_compared": frame_count,
        "bytes_compared_excluding_rwlock": comparable,
        "equal_bytes": equal,
        "equal_ratio": (equal / comparable) if comparable else 1.0,
        "bitwise_identical": first is None and left_header == right_header and len(left) == len(right),
        "first_difference": first,
    }
    print(json.dumps(result, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit(f"用法: {sys.argv[0]} <left.bin> <right.bin>")
    main(Path(sys.argv[1]), Path(sys.argv[2]))
