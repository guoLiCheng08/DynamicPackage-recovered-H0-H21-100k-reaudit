#!/usr/bin/env bash
# DynamicPackage 恢复工程回归入口：不修改原 ELF；不在此脚本中启动原 ELF。
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

python3 analysis/diff_golden.py \
  golden/original_zero_40f_25ms.bin \
  golden/original_zero_repeat_40f_25ms.bin \
  > analysis/regression_golden_determinism.json

make clean
make all
make selftest

gcc -std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror \
  -fno-fast-math -ffp-contract=off -Isrc \
  analysis/telemetry_layout_selftest.c src/dynamic_telemetry.c \
  -o build/telemetry_layout_selftest
build/telemetry_layout_selftest

gcc -std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror \
  -fno-fast-math -ffp-contract=off -Isrc \
  analysis/sensor_selftest.c src/dynamic_math.c src/dynamic_sensors.c -lm \
  -o build/sensor_selftest
build/sensor_selftest

gcc -std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror \
  -fno-fast-math -ffp-contract=off -Isrc \
  analysis/ipc_telemetry_selftest.c src/dynamic_telemetry.c src/dynamic_ipc_telemetry.c \
  -o build/ipc_telemetry_selftest
build/ipc_telemetry_selftest

gcc -std=c11 -O0 -g -Wall -Wextra -Wpedantic -Werror \
  -fno-fast-math -ffp-contract=off -Isrc \
  analysis/flex_inertia_snapshot_compare.c src/dynamic_math.c -lm \
  -o build/flex_inertia_snapshot_compare
build/flex_inertia_snapshot_compare > analysis/flex_inertia_snapshot_compare.txt

sha256sum input/DynamicPackage.elf build/libdynamicpackage_recovered.a \
  > analysis/regression_artifacts.sha256

printf '%s\n' 'PASS: 金标确定性、静态库构建、数学 ABI、刚柔耦合惯量金标、遥测布局、传感器模型和 IPC 序列化自检均已通过。'
printf '%s\n' 'NOTE: 端到端输出对比在完整 CoreDynamic/设备/传感器数值实现完成后启用。'
