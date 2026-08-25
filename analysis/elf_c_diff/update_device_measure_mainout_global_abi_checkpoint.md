# UpdateDeviceMeasure / UpdateMainOut 全局 ABI 检查点

> **验证边界。** 下述结论只对应明确记录的原 ELF 快照和固定随机种子场景，不表示任意设备配置、任意随机状态或完整输入空间均已证明等价。工作仅用于离线仿真研究。

## 反汇编审计

| 原入口 | 地址与大小 | ABI | 审计出的关键行为 |
|---|---:|---|---|
| `UpdateDeviceMeasure` | `0x39b0`, 0x282 | `rdi`：CoreDynamic 0x148 输出对象 | 从 +0x000/+0x020 读取姿态/角速度，从 +0x038/+0x050/+0x068 读取太阳/磁环境，从 +0x098/+0x0b0 读取 GCI 位置/速度，从 +0x0c8 读取时间；调用顺序为 STS 四元数、STS 有效性、陀螺、DSS、DSS 有效性、磁强计、GPS。 |
| `UpdateMainOut` | `0x3c40`, 0x3d9 | `rdi`：0x220 输出帧；`rsi`：CoreDynamic 0x148 输出对象 | 打包 STS/Gyro/DSS/MagMeter/飞轮/SADA/GPS；以 GPS_Kalman +0x28/+0x40 而非传播状态生成 ECEF 与轨道根数；SADA 写入顺序为 current[0]、angular_velocity[0]、current[1]、angular_velocity[1]。 |

## 已发布实现

`src/dynamic_measure_globals.c` 定义可重定位的 `DeviceMeasure` 全局对象，并将 CoreDynamic 输出映射到已通过函数级金标的 `dp_update_device_measure_recovered`。这是一个明确的全局 ABI 桥接，descriptor 的数据指针在每次装载原始对象快照后通过 `dp_device_measure_globals_relocate()` 恢复为本地 backing。

`src/dynamic_mainout_globals.c` 从 `DeviceMeasure`、`RWheel`、`SADA` 和 CoreDynamic 时间字段构造 `DpTelemetrySourceSnapshot`，再复用已验证的封包函数。静态审计和金标共同确认：坐标转换及 `PosVel2Elements_M` 必须使用 GPS 输出状态；此前错误地使用 +0x098/+0x0b0 传播状态时，ECEF 48 字节和轨道 63/64 字节不一致，修正后均为零差异。

## 原 ELF 金标结果

| 比较器 | 被验证的可观察范围 | 结果 |
|---|---|---|
| `update_device_measure_global_gold_compare` | 2 个 Gyro、2 个 MagMeter、3 个 STS 有效性与测量四元数、2 个 DSS 测量/导出角/有效性、GPS 位置速度和 6 个时间整数字段 | 所有枚举区域 0 字节差异 |
| `update_mainout_global_deterministic_compare` | 真实 `dyn_main` 第一步快照中 +0x060..+0x21f 的 448 字节：STS 有效性、Gyro、DSS、MagMeter、飞轮、SADA、GPS、时间前缀、ECEF、轨道根数 | 0/448 不一致 |
| `update_mainout_global_seeded_sts_compare` | `srand(1)` 下连续 3 次 STS 随机四元数输出，分别写入 +0x000/+0x020/+0x040 | 原 ELF 金标逐位通过 |

真实 `dyn_main` 第一步输出快照本身没有保存调用 `UpdateMainOut` 时 libc PRNG 的内部状态。因此其中 +0x000..+0x05f 的 96 字节 STS 噪声四元数不能作为隔离回放的确定性输入；该隐式状态依赖未被隐瞒，而是拆分为固定 `srand(1)` 的连续三台 STS 原 ELF 金标。其余 448 字节与同一真实快照逐字节一致。

## 持续回归

上述三个比较器已纳入 `make CC=gcc AR=ar selftest`。纳入后全量严格自检通过，历史 64 条 `PASS` 标签保持通过。

## 未覆盖边界

尚需构造真实同进程随机状态的端到端快照，覆盖传感器噪声前序消耗随机数、不同 STS/DSS/Gyro/MagMeter 噪声组合及设备初始化默认值。完整 `dyn_init` 包装层和多步 IPC 运行仍属于后续端到端阶段。
