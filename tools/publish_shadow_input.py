#!/usr/bin/env python3
"""发布一帧纯 C 影子输入；序号最后写入，避免消费者读到半帧。"""

import argparse
import mmap
import os
import struct

MAGIC = 0x43534950
VERSION = 1
PATH = "/dev/shm/cfs_test_shadow_input"
FRAME_SIZE = 144


def main() -> int:
    parser = argparse.ArgumentParser(description="发布纯C影子模型输入帧")
    parser.add_argument("sequence", type=int)
    parser.add_argument("--step-time", type=float, default=0.01)
    parser.add_argument("--wheel", type=float, nargs=4, default=[0.0] * 4)
    parser.add_argument("--mtq", type=float, nargs=6, default=[0.0] * 6)
    parser.add_argument("--sada-flag", type=int, default=0)
    parser.add_argument("--sada-angle", type=float, nargs=2, default=[0.0, 0.0])
    parser.add_argument("--thruster-status", type=int, default=0)
    parser.add_argument("--inertia-flag", type=int, default=0)
    args = parser.parse_args()
    if args.sequence <= 0 or args.sequence > 0xFFFFFFFF:
        parser.error("sequence 必须为 1..4294967295")

    command = struct.pack(
        "8x4d6dII2dII", *args.wheel, *args.mtq, args.sada_flag, 0,
        *args.sada_angle, args.thruster_status, args.inertia_flag)
    if len(command) != 120:
        raise RuntimeError(f"命令帧大小错误: {len(command)}")
    fd = os.open(PATH, os.O_RDWR | os.O_CREAT, 0o600)
    os.ftruncate(fd, FRAME_SIZE)
    with mmap.mmap(fd, FRAME_SIZE, access=mmap.ACCESS_WRITE) as frame:
        frame[0:16] = struct.pack("4I", MAGIC, VERSION, 0, 0)
        frame[16:24] = struct.pack("d", args.step_time)
        frame[24:144] = command
        frame.flush()
        frame[8:12] = struct.pack("I", args.sequence)
        frame.flush()
    os.close(fd)
    print(f"1. 结果=通过")
    print(f"2. 输入序号={args.sequence}")
    print(f"3. 步长={args.step_time:.17g}")
    print(f"4. 输入帧={PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
