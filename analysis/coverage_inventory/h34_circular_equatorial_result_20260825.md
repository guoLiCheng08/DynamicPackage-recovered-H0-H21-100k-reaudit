# H34：圆赤道根数初始化与主传播边界

**日期：** 2026-08-25

**范围：** 当前 Linux x86-64、当前运行时、默认严格 C11 构建；仅离线研究、受控仿真与回归。

H34 补充此前高偏心、近圆但非零偏心等路径未直接覆盖的**严格圆轨道与严格赤道轨道**根数退化面。恢复端的 `PosVel2Elements_M` 已有圆轨道 `e <= 1e-14` 与赤道 `abs(sin(i)) <= 1e-14` 的特例分支；本轮不据此改写 C，而是先用原 ELF 验证一个正常量级、有限、可重复的跨启动链合同。正式 `input/DynamicPackage.elf` 保持 mode `0400`，SHA-256 始终为 `312678a62afcb64ff9d198d7311db39bece432702cdfb00830d8439d24906403`；动态调试只针对执行副本。

## 固定合同与预筛

调用顺序为 `DynamicDllInit → 覆写初始根数 → dyn_init → srand(1) → dyn_main`。初始 caller `core` 缓冲为零、设备命令为零、共享帧为新的零初始化映射。根数与其余默认初始化量如下。

| 输入项 | 固定值 | 目的 |
|---|---:|---|
| 半长轴 `a` | `7,000,000 m` | 保持近地轨道量级与有限重力场输入 |
| 偏心率 `e` | `0` | 触及圆轨道退化面 |
| 倾角 `i` | `0 rad` | 触及赤道轨道退化面 |
| RAAN | `0.73 rad` | 保持有限 ABI 输入；在赤道退化时不宣称其物理唯一性 |
| 近地点幅角 `ω` | `1.17 rad` | 保持有限 ABI 输入；在圆退化时不宣称其物理唯一性 |
| 平近点角 `M` | `2.41 rad` | 选择非零有限相位 |
| 随机种子 | `srand(1)` | 固定默认传感器相关随机序列 |

原 ELF 单步预筛导出了初始化 `y`、一步 caller state、global `y`、main 与 IPC payload。五个块的 float/double 文本扫描均未出现 `NaN` 或 `Inf`。初始化后的 `y[7..12]` 为有限 GCI 位置速度，说明该根数合同确实经过 `Elements2PosVel_M` 进入了主传播状态，而不是停留在未消费配置字段。

## 双采集与恢复端逐步差分

100 步与 1000 步均使用 first/second 独立原 ELF probe；每种长度均导出 caller `CoreDynamic` 前 264 B、global `y[33]` 264 B、main telemetry 544 B 和 IPC payload 3000 B。八组 gold 流逐文件 `cmp` 全部 PASS。恢复端使用同一启动链和根数覆盖合同，在每一步比较四块；100 步共 400 条、1000 步共 4000 条比较均为 0-byte mismatch。

| 长度 | state SHA-256 | global-y SHA-256 | main SHA-256 | IPC SHA-256 | 恢复端 |
|---:|---|---|---|---|---|
| 100 | `0020b6ef808f8147fd134b43b016376043b792ea92b1bd266964548171e96e60` | `33bc14549e803ef2008f0ea7ed7fa68f6047bb93a26b1c7208d4c04562a85af0` | `ea40e249f39358c85f944f3024cd2eab632e745ab762ad88b939ef8ba5e486f7` | `46f0acf67347c0d7474aaf0ce30308cca0209fb3638556ad1d37f6ce4ac8b302` | 400 条 PASS |
| 1000 | `7ff99e926fa995ecd50ef5e9998985e26f780818089a261c1fe17e07d940909a` | `55466b7bbcaf035ba642a8d8e3a4b123919a7d479ba5d977ce0f3e0efe89876f` | `1ad67a6c114afe29b722530c8a47907f2f5ea16793c2b650256c89c9a2db8b6d` | `d9d5f6aa1cea010253600e19f8fc81510b052b96c342ecd7ab968d0ae68afe1f` | 4000 条 PASS |

证据文件包括：候选审计 `h34_math_boundary_candidate_audit_20260825.md`、原 ELF 预筛 probe `dyn_main_h34_circular_equatorial_prescreen_probe.gdb`、100/1000 步 first/second probes、恢复端比较器 `analysis/dyn_main_h34_circular_equatorial_{hundred,thousand}_step_compare.c`。`make check-h34-circular-equatorial` 已通过，并已加入默认 `selftest`。

从空构建运行 `make clean && make selftest` 后，默认门禁为 340 个测试可执行项、367 条编译调用、331 条 PASS、非零 mismatch 计数 0、显式失败计数 0。可提交的摘要日志为 `analysis/coverage_inventory/h34_full_selftest_summary_20260825.log`；完整运行输出不纳入版本库，避免重复的超大日志。

## 结论与边界

H34 证明：在上述**唯一列明**的启动链、有限圆赤道根数、默认设备与传感器条件、当前平台及编译标志下，恢复 C 与原 ELF 在 100/1000 步的四个观测块逐字节一致。它扩展了已证实范围，但不构成所有圆轨道、所有赤道轨道、RAAN/近地点幅角的任意处理、近抛物 `e≈1`、近零半径、其他奇异输入、并发、异常资源条件、其他平台或编译器的全域证明。

工程只限离线研究、教学、受控仿真与回归。不得用于真实卫星飞行、姿态控制、实体执行机构、任务决策或任何安全关键用途。
